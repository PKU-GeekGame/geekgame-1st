from rsa import *
from flag import flag1,flag2,validate
import os
import random

certs={}
def register(name,key):
    mcert=getcert(name,key)
    certs[name]=mcert
    return mcert

def doGod():
    print("My RSA N is: "+str(rsa_key[0]))
    print("My RSA e is: "+str(rsa_key[1]))
    print("What is your name?")
    name=bytes.fromhex(input().strip())
    print("What is your key?")
    key=bytes.fromhex(input().strip())
    if name in certs:
        print("I can not issue multiple certificates to one person.")
        return
    if len(certs)>=5:
        print("I can not remember more than 5 people.")
        return
    result=register(name,key)
    print("Your certificate is:")
    print(result)

def doRichard():
    print(MESenc(pad(("Hello, Alice! I will give you two flags. The first is: "+flag1(token)).encode("utf-8")),Public_key).hex())
    print(MESenc(pad(("Sorry, I forget to verify your identity. Please give me your certificate.").encode("utf-8")),Public_key).hex())
    Acert=int(input().strip())
    sinfo=dec(Acert)
    print("SINFO")
    print(sinfo)
    akey=unpackmess(sinfo)
    print("AKEY")
    print(akey)
    print(len(sinfo)-len(akey)-2)
    pinfo=sinfo[:len(sinfo)-len(akey)-2]
    aname=unpackmess(pinfo)
    if aname!=b"Alice":
        print("Who are you?!")
        return
    old_key=int.from_bytes(Public_key,'big')
    comm_key=pow(int.from_bytes(akey,'big'),int.from_bytes(Richard_key,'big'),P)
    for i in range(128*8):
        if (comm_key>>i)&1:
            old_key=(old_key^(old_key<<(i+1)))%(2**2048)
    new_key=int.to_bytes(old_key,256,'big')
    print(MESenc(pad(("I can give you the second flag now. It is: "+flag2(token)).encode("utf-8")),new_key).hex())
    return
        

print("Please input your token:")
token=input().strip()
if not validate(token):
    print("Wrong token!")
    exit(0)

Richard_key=os.urandom(128)
Richard_cert=register(b"Richard",pow(G,int.from_bytes(Richard_key,'big'),P).to_bytes(128,'big'))
Alice_key=os.urandom(128)
Alice_cert=register(b"Alice",pow(G,int.from_bytes(Alice_key,'big'),P).to_bytes(128,'big'))

Public_key=os.urandom(256)

while True:
    print("What do you want to do?")
    print("0. Talk to God.")
    print("1. Talk to Richard.")
    try:
        x=int(input().strip())
    except:
        continue
    if not x in [0,1]:
        print("Wrong input!")
        continue
    if x==0:
        doGod()
    else:
        doRichard()