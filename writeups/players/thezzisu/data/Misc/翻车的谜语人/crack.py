import binascii


def xor_each(k, b):
    assert len(k) == len(b)
    out = []
    for i in range(len(b)):
        out.append(b[i] ^ k[i])
    return bytes(out)


# key = b'\xc4\x07[\xe5zy}b3\x1aM\xed\t\x14\x1c\xea\x8f\xfb\xe52\\\x80\xb1\x98\x8a\xb4\xa6\xdd;\x92X\x81\xcd\x86\x86\xc4\xe0v'
key = b'\x1e\xe0[u\xf2\xf2\x81\x01U_\x9d!yc\x8e\xce[X\r\x04\x94\xbc9\x1d\xd7\xf8\xde\xdcd\xb2Q\xa3\x8a?\x16\xe5\x8a9'

encoded_flag1 = binascii.unhexlify(
    '788c3a1289cbe5383466f9184b07edac6a6b3b37f78e0f7ce79bece502d63091ef5b7087bc44')

encoded_flag2 = binascii.unhexlify(
    '788c3a128994e765373cfc171c00edfb3f603b67f68b087eb69cb8b8508135c5b90920d1b344')

print(xor_each(key, encoded_flag1))
print(xor_each(key, encoded_flag2))
