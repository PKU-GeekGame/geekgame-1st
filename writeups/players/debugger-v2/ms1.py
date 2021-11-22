
def tempering(y):
    y ^= (y >> 11)
    y ^= (y <<  7) & 0x9d2c5680
    y ^= (y << 15) & 0xefc60000
    y ^= (y >> 18)
    return y

def untempering(y):
    y ^= (y >> 18)
    y ^= (y << 15) & 0xefc60000
    y ^= ((y <<  7) & 0x9d2c5680) ^ ((y << 14) & 0x94284000) ^ ((y << 21) & 0x14200000) ^ ((y << 28) & 0x10000000)
    y ^= (y >> 11) ^ (y >> 22)
    return y

def generate(v_624,v_623,v_227):
    y = (v_624 & 0x80000000) | (v_623 & 0x7fffffff)
    return v_227 ^ (y >> 1) ^ [0x0, 0x9908b0df][y & 0x1]

def codes(tables):
    tables=[i=="*" for i in tables.replace("\n","").replace(" ","")]
    v=[]
    for j in range(8):
        v.append(sum([tables[j*32+k]*(2**k) for k in range(32)]))
    return v

yn=input("y or n").strip()=="y"
from pwn import *
conn=remote("prob09.geekgame.pku.edu.cn",10009)

conn.sendline("19:MEUCIQC+0ZqalRVOpWGofI1Vj4wCoWXQxESMGdpvUl/WkYencQIgFCQY9RHopsjB83NKRp0ulbPxJ4EEQMg9PGoWxF4dE2o=")
conn.sendline("y" if yn else "n")
conn.recvline()
conn.recvline()
allcodes=[]
for l in range(78):
    while 1:
        conn.sendline(str(random.randint(0,15))+" "+str(random.randint(0,15)))
        u=conn.recvline()
        if b"BOOM" in u:
            break
    z=[]
    for i in range(16):
        z.append(conn.recvline())
    curcode=codes((b"".join(z)).decode())
    allcodes=allcodes+curcode
    conn.recvline()
    conn.sendline("y")
    conn.recvline()
    print(l)

while 1:
    l=l+1
    print(l)
    k=0
    for i in range(8):
        k=k+tempering(generate(untempering(allcodes[i-(312 if yn else 624)]),untempering(allcodes[i-(311 if yn else 623)]),untempering(allcodes[i-(115 if yn else 227)])))*(2**(32*i))
    ok=1
    for i in range(16):
        for j in range(16):
            if((k >> (i * 16 + j)) & 1==0):
                conn.sendline(str(i)+" "+str(j))
                u=conn.recvline()
                if b"BOOM" in u:
                    ok=0
                    break
        if ok==0:
            break
    if ok:
        conn.interactive()
    else:
        z=[]
        for i in range(16):
            z.append(conn.recvline())
        curcode=codes((b"".join(z)).decode())
        allcodes=allcodes+curcode
        conn.recvline()
        conn.sendline("y")
        conn.recvline()
        
