from Crypto.Util.number import bytes_to_long,long_to_bytes

def pad(msg):
  n = 32 - len(msg) % 32
  return msg + bytes([n]) * n

sdata = 'b9a4a845487fa3818f263b862d57270889db4be4d1cffb9ade88755e6011451abdb9e4401b22ea91866725921e12622caa830de18cd9e1e59bf4604a5212206c918caa76274f8c8ae20e4ae1507f5b779df369e49fb6a0a7efbb0c03527c4321'

pmess = bytes.fromhex(sdata)
smess = pad(("Hello, Alice! I will give you two flags. The first is: ").encode("utf-8"))

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

fdata = bytes.fromhex(sdata)
boom = b''
for it in range(0,len(fdata),32):
  pmess=fdata[it:it+32]
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