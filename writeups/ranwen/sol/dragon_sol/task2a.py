from typing import get_origin
import requests
import gmpy2
import sys

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

# def getblock(type,bx,by):
#     obj=balls[type]
#     rng=JavaRandom(obj[2]*bx+obj[3]*by+obj[4]*myseed+obj[5])
#     posa=rng.nextInt(obj[1])
#     posb=rng.nextInt(obj[1])
#     blsx[(bx,by)]=(posa,posb)
#     return (posa,posb)

def getblock(type,bx,by):
    obj=balls[type]
    bls=obj[1]
    bxb=bx*bls//32
    bxe=((bx+1)*bls-1)//32
    byb=by*bls//32
    bye=((by+1)*bls-1)//32
    for i in range(bxb,bxe+1):
        for j in range(byb,bye+1):
            mapi=explore(i*32,j*32)['materials']
            #print(mapi)
            for px in range(32):
                for py in range(32):
                    if mapi[px][py]==obj[0]:
                        rposx=(i*32+px)
                        rposy=(j*32+py)
                        if rposx//bls==bx and rposy//bls==by:
                            blsx[(bx,by)]=(rposx%bls,rposy%bls)
                            return (rposx%bls,rposy%bls)

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
SAMP=20
tmod=balls[0][1]

orl=[]

for ww in range(48):
    print(ww,file=sys.stderr)
    if ww>35:
        orl.append(None)
        continue
    pdl=[]
    for i in range(SAMP):
        dx,dy=getdelta(i*(2**ww),balls[0][2],balls[0][3])
        #print(dx,dy,file=sys.stderr)
        pdl.append(getblock(0,dx,dy)[0])
    
    nps=[0,0]
    nw=(multi>>ww)&1
    pw=2**ww
    for i in range(1,SAMP):
        # print(pdl[i-1])
        # print(pdl[i])
        adlv=(pdl[i-1]+((multi*pw%almod)>>17))%tmod
        adlv=(adlv,(adlv+1)%tmod)
        delv=(pdl[i-1]-((multi*pw%almod)>>17))%tmod
        delv=(delv,(delv-1)%tmod)
        if pdl[i] in adlv:
            nps[(i+1+nw)%2]+=1
        if pdl[i] in delv:
            nps[(i+nw)%2]+=1
    #print(nps)

    npss=[0,0]
    for i in range(1,SAMP):
        # print(pdl[i-1])
        # print(pdl[i])
        adlv=(pdl[i-1]+((multi*pw%almod)>>17))%tmod
        adlv=(adlv,(adlv-2**31)%tmod)
        delv=(pdl[i-1]-((multi*pw%almod)>>17))%tmod
        delv=(delv,(delv+2**31)%tmod)
        if pdl[i] in adlv:
            npss[(i+1+nw)%2]+=1
        if pdl[i] in delv:
            npss[(i+nw)%2]+=1
    #print(npss)
    nw=None
    if ww<12:
        if nps[0]>SAMP//2*4//5 and nps[1]<SAMP//2*3//5:
            nw=0
        elif nps[1]>SAMP//2*4//5 and nps[0]<SAMP//2*3//5:
            nw=1
    if ww>16:
        if npss[0]>SAMP//2*4//5 and npss[1]<SAMP//2*3//5:
            nw=0
        elif npss[1]>SAMP//2*4//5 and npss[0]<SAMP//2*3//5:
            nw=1
    #print(nw)
    orl.append(nw)

nucnt=0
newp=[]
basev=0
for i in range(len(orl)):
    x=orl[i]
    if x==None:
        nucnt+=1
        newp.append(i)
    else:
        basev+=x*(2**i)
#print(nucnt)
print(len(orl))
for x in orl:
    if x==None:
        print('-1')
    else:
        print(x)
print(len(blsx))
for k,v in blsx.items():
    print(k[0],k[1],v[0],v[1])

# nucnt=18

# for st in range(2**nucnt):
#     myv=basev
#     for i in range(nucnt):
#         myv+=((st>>i)&1)*(2**newp[i])

#print(explore(0,0))