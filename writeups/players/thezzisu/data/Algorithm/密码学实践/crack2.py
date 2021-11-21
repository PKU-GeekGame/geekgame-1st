from Crypto.Util.number import bytes_to_long,long_to_bytes

def pad(msg):
  n = 32 - len(msg) % 32
  return msg + bytes([n]) * n

sdata = '0c8fc537bab12e0e9e67beafd8d7d4d75490dc84c2b2d1eb7f80b6f714fc60bb36b9841beea11e0d8f2bac8ae9dc9dda48f1d0e9b1f6a6e86b93a4cd26e36eb90797eb4ad1dc4f6ef449d6cdb7befab722dcf8ee82a09f8d0efbb383669026c6'

pmess = bytes.fromhex(sdata)
smess = pad(("I can give you the second flag now. It is: ").encode("utf-8"))

a = bytes_to_long(pmess[0:8])
b = bytes_to_long(pmess[8:16])
c = bytes_to_long(pmess[16:24])
d = bytes_to_long(pmess[24:32])

_a = bytes_to_long(smess[0:8])
_b = bytes_to_long(smess[8:16])
_c = bytes_to_long(smess[16:24])
_d = bytes_to_long(smess[24:32])

key1 = a ^ _c
key2 = b ^ _d
key3 = c ^ _a ^ _c
key4 = d ^ _b ^ _d

fmess = bytes.fromhex(sdata)
boom = b''
for it in range(0,len(fmess),32):
  pmess=fmess[it:it+32]
  a = bytes_to_long(pmess[0:8])
  b = bytes_to_long(pmess[8:16])
  c = bytes_to_long(pmess[16:24])
  d = bytes_to_long(pmess[24:32])
  a, b, c, d = c ^ key3 ^ a ^ key1, d ^ key4 ^ b ^ key2, a ^ key1, b ^ key2
  a=long_to_bytes(a,8)
  b=long_to_bytes(b,8)
  c=long_to_bytes(c,8)
  d=long_to_bytes(d,8)
  boom+=a+b+c+d

print(boom)