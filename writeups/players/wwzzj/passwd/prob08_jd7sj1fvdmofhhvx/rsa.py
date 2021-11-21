from typing import List
from Crypto.PublicKey import RSA
from Crypto.Util.number import bytes_to_long, long_to_bytes
from hashlib import sha256
import base64

key = RSA.generate(2048)
rsa_key = (key.n, key.e)
rsa_d = key.d


def packmess(mess):
    assert len(mess) <= 65535
    return mess+(len(mess).to_bytes(2, 'big'))


def unpackmess(mess):
    rlen = int.from_bytes(mess[-2:], 'big')
    if rlen > len(mess)-2:
        mess = b'\x00'*(rlen-len(mess)+2)+mess
    return mess[-(2+rlen):-2]


def getcert(name, key):
    rmess = packmess(name)+packmess(key)
    return enc(rmess)


def verify(cert):
    rmess = dec(cert)
    rkey = unpackmess(rmess)
    nmess = rmess[:-(len(rkey)+2)]
    rname = unpackmess(nmess)
    return (rname, rkey)


def enc(mess):
    gmess = int.from_bytes(mess, 'big')
    cip = pow(gmess, rsa_d, rsa_key[0])
    return cip


def dec(cip):
    mess = pow(cip, rsa_key[1], rsa_key[0])
    messm = mess.to_bytes((mess.bit_length()+7)//8, 'big')
    return messm


def pad(msg):
    n = 32 - len(msg) % 32
    return msg + bytes([n]) * n


def unpad(msg):
    assert len(msg) > 0 and len(msg) % 32 == 0
    n = msg[-1]
    assert 1 <= n <= 32
    assert msg[-n:] == bytes([n]) * n
    return msg[:-n]


def MESenc(mess: bytes, skey: bytes):
    assert len(skey) == 8*32
    keys = [bytes_to_long(skey[i*8:(i+1)*8]) for i in range(32)]
    assert len(mess) % 32 == 0
    cip = b""
    for it in range(0, len(mess), 32):
        pmess = mess[it:it+32]
        a = bytes_to_long(pmess[0:8])
        b = bytes_to_long(pmess[8:16])
        c = bytes_to_long(pmess[16:24])
        d = bytes_to_long(pmess[24:32])
        for key in keys:
            a, b, c, d = b, c, d, a ^ c ^ key
        a = long_to_bytes(a, 8)
        b = long_to_bytes(b, 8)
        c = long_to_bytes(c, 8)
        d = long_to_bytes(d, 8)
        cip += a+b+c+d
    return cip


P = 90217665064898209874945440415607645619739552209209829698859302616784804764535733761793578496057231124307702060703777047113648993244620436073319613409562635176198907683514151487667596066485528729421061091099275256268502121631878101098253283229895343146346757918637948217208979264467987857303672675468072062419
G = 13
