from Crypto.Util.number import bytes_to_long,long_to_bytes

def pad(msg):
    n = 32 - len(msg) % 32
    return msg + bytes([n]) * n

def MESenc(mess:bytes):
    assert len(mess)%32 == 0
    cip=b""
    for it in range(0,len(mess),32):
        pmess=mess[it:it+32]
        a = bytes_to_long(pmess[0:8])
        b = bytes_to_long(pmess[8:16])
        c = bytes_to_long(pmess[16:24])
        d = bytes_to_long(pmess[24:32])
        for _ in range(32):
            a, b, c, d = b, c, d, a ^ c
        a=long_to_bytes(a,8)
        b=long_to_bytes(b,8)
        c=long_to_bytes(c,8)
        d=long_to_bytes(d,8)
        cip+=a+b+c+d
    return cip

def rev_MESenc(mess:bytes):
    assert len(mess)%32 == 0
    cip=b""
    for it in range(0,len(mess),32):
        pmess=mess[it:it+32]
        a = bytes_to_long(pmess[0:8]) ^ keya
        b = bytes_to_long(pmess[8:16]) ^ keyb
        c = bytes_to_long(pmess[16:24]) ^ keyc
        d = bytes_to_long(pmess[24:32]) ^ keyd
        for _ in range(32):
            a, b, c, d = b ^ d, a, b, c
        a=long_to_bytes(a,8)
        b=long_to_bytes(b,8)
        c=long_to_bytes(c,8)
        d=long_to_bytes(d,8)
        cip+=a+b+c+d
    return cip

msg2 = pad(("Sorry, I forget to verify your identity. Please give me your certificate.").encode("utf-8"))


cip1 = int("9ed3c720eb216b0428b5e17516b4256897f178ff400d020b50bb0d6db4d772e5a4e5a60cbf315b0739f9f35027bf6c658b9054923349750844a81f5786c87ce795cbe95d804c0a64429b891779dd0b08e1bd5cb5001f4c6d01c00819c69b3498",16)
cip2 = int("82d9c725eb306d0c35b5fe7602a1256f91b469e9571043231bb85d65a8d226b691df9136ae2f614a35faf26b57b06074b5b823fc025716021bc1526ffbd572ade1a1f0449955137d5b82900e60c41211d5ca4a933f38697e7bfe4b78bac572ff",16)

tmp = MESenc(msg2)
cip2_b = bytes.fromhex(hex(cip2)[2:])
keya = bytes_to_long(tmp[0:8]) ^ bytes_to_long(cip2_b[0:8])
keyb = bytes_to_long(tmp[8:16]) ^ bytes_to_long(cip2_b[8:16])
keyc = bytes_to_long(tmp[16:24]) ^ bytes_to_long(cip2_b[16:24])
keyd = bytes_to_long(tmp[24:32]) ^ bytes_to_long(cip2_b[24:32])

print(rev_MESenc(bytes.fromhex(hex(cip1)[2:])))
