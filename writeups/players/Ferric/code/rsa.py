from pwn import *
from Crypto.Util.number import bytes_to_long, long_to_bytes

token = b''

r = remote('prob08.geekgame.pku.edu.cn', 10008)
r.sendlineafter(b'Please input your token:', token)

for _ in range(4):
    r.recvline()

r.sendline(b'0')
N = int(r.recvline().decode()[13:-1])
e = int(r.recvline().decode()[13:-1])

# [][0000][Alice][0005]
r.recvline()
r.sendline(b'')
r.recvline()
r.sendline(b'Alice'.hex())
r.recvline()
cert = int(r.recvline().decode()[:-1])

for _ in range(3):
    r.recvline()
r.sendline(b'0')
for _ in range(3):
    r.recvline()

# [00][0001][][0000]
r.sendline(b'00')
r.recvline()
r.sendline(b'')
r.recvline()
cert2 = int(r.recvline().decode()[:-1])

forge = cert * cert2 % N

for _ in range(3):
    r.recvline()
r.sendline(b'1')
r.recvline()  # Hello Alice
r.recvline()  # Sorry, I forget
r.sendline(str(forge).encode())
k = r.recvline().decode()[:-1]  # I can give
enc = bytes.fromhex(k)
penc = enc[:32]
pmess = b'I can give you the second flag n'

key0 = bytes_to_long(pmess[16:24]) ^ bytes_to_long(penc[0:8])
key1 = bytes_to_long(pmess[24:32]) ^ bytes_to_long(penc[8:16])
key2 = bytes_to_long(pmess[0:8]) ^ bytes_to_long(
    pmess[16:24]) ^ bytes_to_long(penc[16:24])
key3 = bytes_to_long(pmess[8:16]) ^ bytes_to_long(
    pmess[24:32]) ^ bytes_to_long(penc[24:32])

decip = b''
for it in range(0, len(enc), 32):
    c = bytes_to_long(enc[it:it+8]) ^ key0
    d = bytes_to_long(enc[it+8:it+16]) ^ key1
    a = bytes_to_long(enc[it+16:it+24]) ^ key2 ^ c
    b = bytes_to_long(enc[it+24:it+32]) ^ key3 ^ d
    decip += long_to_bytes(a) + long_to_bytes(b) + \
        long_to_bytes(c) + long_to_bytes(d)

print(decip)
