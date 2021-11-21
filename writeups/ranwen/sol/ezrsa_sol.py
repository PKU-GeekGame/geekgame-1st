import os
from Crypto.Util.number import bytes_to_long, long_to_bytes
from typing import *
import sympy
from pwn import *

#s=process("python3 server.py",shell=True)
s=remote('prob08.geekgame.pku.edu.cn',10008)

token="TOKEN"

keys = [sympy.symbols("key_"+str(i)) for i in range(32)]
a,b,c,d = sympy.symbols("a,b,c,d")
for key in keys:
    a, b, c, d = b, c, d, a ^ c ^ key
print(a)
print(b)
print(c)
print(d)

def decrypt(cip,messpre):
    cip=bytes.fromhex(cip)
    messpre=messpre.encode()[:32]
    cippre=[bytes_to_long(cip[i*8:(i+1)*8]) for i in range(4)]
    retv=b""
    for i in range(0,len(cip),32):
        mcip=cip[i:i+32]
        mcip=[bytes_to_long(mcip[i*8:(i+1)*8]) for i in range(4)]
        result=[0,0,0,0]
        xor_cipher = [x ^ y for x,y in zip(mcip, cippre)]
        result[2] = xor_cipher[0] ^ bytes_to_long(messpre[16:24])
        result[3] = xor_cipher[1] ^ bytes_to_long(messpre[24:32])
        result[0] = xor_cipher[2] ^ bytes_to_long(messpre[0:8]) ^ xor_cipher[0]
        result[1] = xor_cipher[3] ^ bytes_to_long(messpre[8:16]) ^ xor_cipher[1]
        retv+=long_to_bytes(result[0])+long_to_bytes(result[1])+long_to_bytes(result[2])+long_to_bytes(result[3])
    return retv

con_mess=b"Alice\x00\x05\x00\x00\x00\x02"

#input1: "" "0000" -> "\x02"

#input2: namex "00" -> con_mess

nmess=long_to_bytes(bytes_to_long(b"Alice\x00\x05\x00\x00\x00\x02")//2)
nmess=nmess[:-3]
nmlen=bytes_to_long(nmess[-2:])

namex=nmess[:-2].rjust(nmlen,b"\x00").hex()

#print(namex)
print(nmlen)
s.sendline(token.encode())
s.sendline(b"0")
s.recvuntil(b"My RSA N is: ")

N=int(s.recvline().strip().decode())

s.sendline(b"")
s.sendline(b"0000")
s.recvuntil(b"certificate is:\n")
cert1=int(s.recvline())

s.sendline(b"0")
s.sendline(namex.encode())
s.sendline(b"00")
s.recvuntil(b"certificate is:\n")
cert2=int(s.recvline())

cert3=cert1*cert2%N

print(cert1)
print(cert2)
print(cert3)
print(N)

s.recvuntil(b"Talk to Richard.\n")
s.sendline(b"1")
rcip=s.recvline().strip().decode()
s.recvline()
print(decrypt(rcip,"Hello, Alice! I will give you two flags. The first is:"))

s.sendline(str(cert3).encode())
rcip=s.recvline().strip().decode()
print(decrypt(rcip,"I can give you the second flag now. It is:"))
#s.interactive()



