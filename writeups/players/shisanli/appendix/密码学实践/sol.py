from Crypto.Util.number import bytes_to_long, long_to_bytes
#strings1 = b"621bf387cfbf7c620c34b993347f8c50699a3ed14342b6b751b608fa360f11566606bf829ce235720575a787273ac9544ac278d41e54acc814ca1dee240c74004a13f1b4a08f5369611cc8f44957f02f7d921cd12d3bed8a608571a70462176d"
#strings2 = b"611dbf9d8aaa7c721034b993342dd84e71966cd51057b6af01b904ed70187848721be98ecfb57034107bb58e613c9d55559a26c04510e38e01c00be7231f2c53026588fcf8cf02037e03d7eb5648ef3035e84faf787f9cf261ff12f0620f2c01"
strings2 = b"\x62\x1b\xf3\x87\xcf\xbf\x7c\x62\x0c\x34\xb9\x93\x34\x7f\x8c\x50\x69\x9a\x3e\xd1\x43\x42\xb6\xb7\x51\xb6\x08\xfa\x36\x0f\x11\x56\x66\x06\xbf\x82\x9c\xe2\x35\x72\x05\x75\xa7\x87\x27\x3a\xc9\x54\x4a\xc2\x78\xd4\x1e\x54\xac\xc8\x14\xca\x1d\xee\x24\x0c\x74\x00\x4a\x13\xf1\xb4\xa0\x8f\x53\x69\x61\x1c\xc8\xf4\x49\x57\xf0\x2f\x7d\x92\x1c\xd1\x2d\x3b\xed\x8a\x60\x85\x71\xa7\x04\x62\x17\x6d"
strings1 = b"\x61\x1d\xbf\x9d\x8a\xaa\x7c\x72\x10\x34\xb9\x93\x34\x2d\xd8\x4e\x71\x96\x6c\xd5\x10\x57\xb6\xaf\x01\xb9\x04\xed\x70\x18\x78\x48\x72\x1b\xe9\x8e\xcf\xb5\x70\x34\x10\x7b\xb5\x8e\x61\x3c\x9d\x55\x55\x9a\x26\xc0\x45\x10\xe3\x8e\x01\xc0\x0b\xe7\x23\x1f\x2c\x53\x02\x65\x88\xfc\xf8\xcf\x02\x03\x7e\x03\xd7\xeb\x56\x48\xef\x30\x35\xe8\x4f\xaf\x78\x7f\x9c\xf2\x61\xff\x12\xf0\x62\x0f\x2c\x01"
a1_2 = bytes_to_long(strings1[0:8])
b1_2 = bytes_to_long(strings1[8:16])
c1_2 = bytes_to_long(strings1[16:24])
d1_2 = bytes_to_long(strings1[24:32])
msg0 = "Hello, Alice! I will give you two flags. The first is: "
msg = "Sorry, I forget to verify your identity. Please give me your certificate."
#print(len(msg))
msgu1 = msg[0:32].encode('utf-8')
print(type(msgu1))
print(len(msgu1))
#print(msgu1)

a1_1 = bytes_to_long(msgu1[0:8])
b1_1 = bytes_to_long(msgu1[8:16])
c1_1 = bytes_to_long(msgu1[16:24])
d1_1 = bytes_to_long(msgu1[24:32])

key0 = c1_1 ^ a1_2
key1 = d1_1 ^ b1_2
key2 = a1_1 ^ c1_2 ^ c1_1
key3 = b1_1 ^ d1_2 ^ d1_1

print(len(msg))

msgu2 = msg[32:64].encode('utf-8')
a2_1 = bytes_to_long(msgu2[0:8])
c2_1 = bytes_to_long(msgu2[16:24])
c2_2 = bytes_to_long(strings1[48:56])
key2_2 = a2_1 ^ c2_2 ^ c2_1

print(key2)
print(key2_2)
#print(a)
#print(b)
#print(c)
#print(d)



for i in range(3):
    strings0 = strings2[i*32:32+i*32]
    a2 = bytes_to_long(strings0[0:8])
    b2 = bytes_to_long(strings0[8:16])
    c2 = bytes_to_long(strings0[16:24])
    d2 = bytes_to_long(strings0[24:32])
    c1 = a2 ^ key0
    d1 = b2 ^ key1
    a1 = c2 ^ c1 ^ key2
    b1 = d2 ^ d1 ^ key3
    msgu = long_to_bytes(a1,8) + long_to_bytes(b1,8) + long_to_bytes(c1,8) + long_to_bytes(d1,8)
    print(msgu)
    #msgg = msgu.decode('utf-8')
    #print(msgg)
    

