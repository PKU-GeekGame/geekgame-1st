from typing import get_origin
import requests
import gmpy2

token="TOKEN"

def send_req(path,x,y,post=True):
    url='https://prob16.geekgame.pku.edu.cn/api/'+path
    if post:
        r=requests.post(url,params={'x':x,'y':y,'token':token},data={'x':x,'y':y,'token':token})
    else:
        r=requests.get(url,params={'x':x,'y':y,'token':token})
    #print(r.text)
    return r.json()

def pick(x,y):
    return send_req('break',x,y)

class JavaRandom:
    def __init__(self,seed):
        self.seed=(seed^0x5DEECE66D)&((1<<48)-1)
        #print(self.seed)
    def next(self,bits):
        self.seed=(self.seed*0x5DEECE66D+0xB)&((1<<48)-1)
        return self.seed>>(48-bits)
    def nextInt(self,n):
        if n<=0:
            return 0
        if (n&-n)==n:
            return (n*self.next(31))>>31
        while True:
            bits=self.next(31)
            val=bits%n
            if bits-val+(n-1)>=0:
                return val

myseed=1145141919810

blsx={}

def getblock(type,bx,by):
    obj=balls[type]
    rng=JavaRandom(obj[2]*bx+obj[3]*by+obj[4]*myseed+obj[5])
    posa=rng.nextInt(obj[1])
    posb=rng.nextInt(obj[1])
    blsx[(bx,by)]=(posa,posb)
    return (posa,posb)

checkpoints={}

def explore(x,y):
    rtv=send_req('state',x,y,post=False)
    for i in range(32):
        for j in range(32):
            if rtv['materials'][i][j]!='AIR':
                checkpoints[(i+x//32*32,j+y//32*32)]=rtv['materials'][i][j]
    return rtv

def getpos(x,y):
    jt=explore(x,y)
    return jt['materials'][x%32][y%32]

balls=[("FIRST",29,0x2e01,0x76b5,0x15beccc9,0x7296ea13),
("SECOND",53,0x7c31,0x7edf,0x289223e1,0x4878857f),
("THIRD",193,0x30b9,0x5fcf,0x3857dc53,0x6e3bd3dd),
("FOURTH",1543,0x1159,0x1445,0x18b0a50b,0x27d58de9),
("FIFTH",24593,0x683f,0x42b5,0x1b7a1d5b,0x33f4f1a7),
("SIXTH",393241,0x1d75,0x623b,0x70517261,0x3ac463fb),
("SEVENTH",25165843,0x4799,0x3b11,0x5b07e3b5,0x6b85fa33)]

def getdelta(dtv,mx,my):
    pmx=gmpy2.invert(mx,my)
    apmx=pmx*dtv
    apmy=(dtv-apmx*mx)//my
    subs=apmx//my
    apmx-=subs*my
    apmy+=subs*mx
    assert apmx*mx+apmy*my==dtv
    return (int(apmx),int(apmy))

# for x in balls:
#     for i in range(1,10):
#         getdelta(i,x[2],x[3])
multi=0x5DEECE66D
almod=2**48
#print(bin(multi))
#print(bin((myseed*0x15beccc9+0x7296ea13)&((1<<48)-1)))
#0b10111011110111011001110011001101101
#0b1100010110001000010101000001101011010111100101

result=98350091154102

glseed=int((result-0x7296ea13)*gmpy2.invert(0x15beccc9,almod)%almod)
print(glseed)

for i in range(7):
    obj=balls[i]
    rseed=obj[2]*i+obj[3]*i+obj[4]*glseed+obj[5]
    rng=JavaRandom(rseed)
    px=rng.nextInt(obj[1])
    py=rng.nextInt(obj[1])
    rx=i*obj[1]+px
    ry=i*obj[1]+py
    print(pick(rx,ry))

print(explore(0,0))
