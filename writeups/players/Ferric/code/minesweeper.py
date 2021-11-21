from randcrack import RandCrack
from pwn import *

while True:
    token = b''
    r = remote('prob09.geekgame.pku.edu.cn', 10009)
    r.sendlineafter(b'token: ', token)

    r.sendlineafter(b'easy mode? (y/n)', b'n')

    rc = RandCrack()

    for _ in range(78):
        r.recvuntil(b"New Game!\n")
        board = [[None] * 16 for _ in range(16)]

        f = False
        # want to BOOM!
        for i in range(16):
            for j in range(16):
                r.sendlineafter(b'> ', f'{i} {j}')
                if r.recvline() == b'BOOM!\n':
                    f = True
                    break
            if f:
                break

        # get board
        for i in range(16):
            line = r.recvline()
            for j in range(16):
                board[i][j] = line[j] == ord('*')

        # submit to rc
        for i in range(0, 16, 2):
            bits = 0
            for j in range(15, -1, -1):
                bits = bits * 2 + board[i + 1][j]
            for j in range(15, -1, -1):
                bits = bits * 2 + board[i][j]
            rc.submit(bits)

        r.sendlineafter(b'try again? (y/n)', b'y')

    bits = rc.predict_getrandbits(256)
    board = [[None] * 16 for _ in range(16)]
    # predicted
    for i in range(16):
        for j in range(16):
            x = (bits >> (i * 16 + j)) & 1
            board[i][j] = x
            if x == 0:
                r.sendlineafter(b'> ', f'{i} {j}')
                res = r.recvline()
                if res == b'BOOM!\n':
                    for _ in range(16):
                        print(r.recvline())
                    print('-' * 16)
                    for k in range(16):
                        print(
                            ''.join(map(lambda x: '*' if x else ' ', board[k])))
                    exit(0)
    if r.recvline().startswith(b'You'):
        print(r.recvline())
        break
