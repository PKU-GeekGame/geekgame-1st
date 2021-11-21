# import zwsp_steg
# from Crypto.Random import get_random_bytes
import binascii

"""
key
"b'\\x1e\\xe0[u\\xf2\\xf2\\x81\\x01U_\\x9d!yc\\x8e\\xce[X\\r\\x04\\x94\\xbc9\\x1d\\xd7\\xf8\\xde\\xdcd\\xb2Q\\xa3\\x8a?\\x16\\xe5\\x8a9'"
"""

key = b'\x1e\xe0[u\xf2\xf2\x81\x01U_\x9d!yc\x8e\xce[X\r\x04\x94\xbc9\x1d\xd7\xf8\xde\xdcd\xb2Q\xa3\x8a?\x16\xe5\x8a9'
with open('flag1.txt', 'rb') as f:
    flag1 = binascii.unhexlify(f.readline().strip())
print(flag1)
print(key)
def xor_each(k, b):
    print(len(k),len(b))
    # assert len(k)==len(b)
    out = []
    for i in range(len(b)):
        out.append(b[i]^k[i])
    return bytes(out)

res = xor_each(key,flag1)
print(res)
print(res.decode())
# encoded_flag1 = xor_each(key, flag1.encode())
# encoded_flag2 = xor_each(key, flag2.encode())
