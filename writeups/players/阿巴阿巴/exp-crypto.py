import os
from Crypto.Util.number import bytes_to_long, long_to_bytes
from rsa import *
from pwn import *

token = os.getenv("TOKEN")

def pad(msg):
    n = 32 - len(msg) % 32
    return msg + bytes([n]) * n

def unpad(msg):
    assert len(msg) > 0 and len(msg) % 32 == 0
    n = msg[-1]
    assert 1 <= n <= 32
    assert msg[-n:] == bytes([n]) * n
    return msg[: -n]

class MESenc_cracker:
    def __init__(self, text, encrypted):
        assert len(text) == 32
        assert len(encrypted) == 32
        a = bytes_to_long(text[0: 8])
        b = bytes_to_long(text[8: 16])
        c = bytes_to_long(text[16: 24])
        d = bytes_to_long(text[24: 32])
        A = bytes_to_long(encrypted[0: 8])
        B = bytes_to_long(encrypted[8: 16])
        C = bytes_to_long(encrypted[16: 24])
        D = bytes_to_long(encrypted[24: 32])

        self.keyC = A ^ c
        self.keyD = B ^ d
        self.keyA = a ^ C ^ c ^ self.keyC
        self.keyB = b ^ D ^ d ^ self.keyD
    
    def encrypt32(self, text):
        assert len(text) == 32
        a = bytes_to_long(text[0: 8])
        b = bytes_to_long(text[8: 16])
        c = bytes_to_long(text[16: 24])
        d = bytes_to_long(text[24: 32])
        A, B, C, D = c ^ self.keyC, d ^ self.keyD, a ^ c ^ self.keyA ^ self.keyC, b ^ d ^ self.keyB ^ self.keyD
        A = long_to_bytes(A, 8)
        B = long_to_bytes(B, 8)
        C = long_to_bytes(C, 8)
        D = long_to_bytes(D, 8)
        return A + B + C + D
    
    def decrypt32(self, encrypted):
        assert len(encrypted) == 32
        A = bytes_to_long(encrypted[0: 8])
        B = bytes_to_long(encrypted[8: 16])
        C = bytes_to_long(encrypted[16: 24])
        D = bytes_to_long(encrypted[24: 32])
        c, d = A ^ self.keyC, B ^ self.keyD
        a = C ^ c ^ self.keyA ^ self.keyC
        b = D ^ d ^ self.keyB ^ self.keyD
        a = long_to_bytes(a, 8)
        b = long_to_bytes(b, 8)
        c = long_to_bytes(c, 8)
        d = long_to_bytes(d, 8)
        return a + b + c + d
    
    def encrypt(self, text):
        assert len(text) % 32 == 0
        encrypted = b""
        for i in range(0, len(text), 32):
            encrypted += self.encrypt32(text[i: i + 32])
        return encrypted
    
    def decrypt(self, encrypted):
        assert len(encrypted) % 32 == 0
        text = b""
        for i in range(0, len(encrypted), 32):
            text += self.decrypt32(encrypted[i: i + 32])
        return text

def split_name_key(val):
    sinfo = val.to_bytes(2 * 65536, "big")
    akey = unpackmess(sinfo)
    pinfo = sinfo[:len(sinfo) - len(akey) - 2]
    aname = unpackmess(pinfo)
    return aname, akey, aname.hex(), akey.hex()

# conn = process(["python3", "server.py"])
conn = remote("prob08.geekgame.pku.edu.cn", 10008)
conn.recvuntil("Please input your token:\n")
conn.sendline(token)
conn.recvuntil("1. Talk to Richard.\n")
conn.sendline("0")
conn.recvuntil("My RSA N is: ")
N = int(conn.recvline().strip(b"\n").decode("utf-8"))
conn.recvuntil("My RSA e is: ")
E = int(conn.recvline().strip(b"\n").decode("utf-8"))

_, _, Aname, Akey = split_name_key(5)
_, _, Bname, Bkey = split_name_key(265389176375109180405367184031745)

conn.recvuntil("What is your name?\n")
conn.writeline(Aname)
conn.recvuntil("What is your key?\n")
conn.writeline(Akey)
conn.recvuntil("Your certificate is:\n")
certA = int(conn.recvline().strip(b"\n").decode("utf-8"))

conn.recvuntil("1. Talk to Richard.\n")
conn.sendline("0")
conn.recvuntil("What is your name?\n")
conn.writeline(Bname)
conn.recvuntil("What is your key?\n")
conn.writeline(Bkey)
conn.recvuntil("Your certificate is:\n")
certB = int(conn.recvline().strip(b"\n").decode("utf-8"))
certAlice = certA * certB % N

conn.recvuntil("1. Talk to Richard.\n")
conn.sendline("1")
a = conn.recvline().decode("utf-8").rstrip("\n")
b = conn.recvline().decode("utf-8").rstrip("\n")
a, b = bytes.fromhex(a), bytes.fromhex(b)

cracker = MESenc_cracker(b"Hello, Alice! I will give you tw", a[:32])
print(unpad(cracker.decrypt(a)))
conn.sendline("%d" % certAlice)
decrypted = conn.recvline().decode("utf-8").rstrip("\n")
decrypted = bytes.fromhex(decrypted)

cracker = MESenc_cracker(b"I can give you the second flag n", decrypted[:32])
print(unpad(cracker.decrypt(decrypted)))

quit(0)

from sympy.ntheory import factorint

def factor(val):
    keys = factorint(val)
    g = min(keys.keys())
    return g, val // g

import os
while True:
    key_len = 5
    name = b"Alice\x00\x05"
    NAME = int.from_bytes(name, "big")
    key = os.urandom(key_len)
    KEY = int.from_bytes(key, "big")
    data = NAME * 256 ** (key_len + 2) + KEY * 65536 + key_len
    buffer = name + key + key_len.to_bytes(2, "big")
    assert data == int.from_bytes(buffer, "big")
    print(data)
    a, b = factor(data)
    if a >= 65535:
        continue
    b_key_len = b & 65535
    print(b_key_len)
    try:
        w = 1000
        B = b.to_bytes(w, "big")
        if b_key_len + 2 >= w:
            continue
        B = B[:-b_key_len - 2]
        b_name_len = int.from_bytes(B[-2:], "big")
        if b_name_len == 0:
            continue
    except:
        continue
    print(B)
    print(a, b, data)
    print(b_key_len)
    break