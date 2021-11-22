import requests, asyncio, aiohttp, javarandom

def getpq(n):
    k=1922492947-n
    p=(22266*k)%30389
    q=(k-11777*p)//30389
    return (-p,-q)

async def get(i,session):
    p,q=getpq(i)
    p=p*29
    q=q*29
    p1=p//32*32
    q1=q//32*32
    async with session.get("https://prob16.geekgame.pku.edu.cn/api/state?token=19%3AMEUCIQC%2B0ZqalRVOpWGofI1Vj4wCoWXQxESMGdpvUl%2FWkYencQIgFCQY9RHopsjB83NKRp0ulbPxJ4EEQMg9PGoWxF4dE2o%3D&x="+str(p1)+"&y="+str(q1)) as response:
        data=await response.json()
        for ii in range(32):
            for jj in range(32):
                if data["materials"][ii][jj]=="FIRST" and p<=p1+ii<p+29 and q<=q1+jj<q+29:
                    return p1+ii-p
    async with session.get("https://prob16.geekgame.pku.edu.cn/api/state?token=19%3AMEUCIQC%2B0ZqalRVOpWGofI1Vj4wCoWXQxESMGdpvUl%2FWkYencQIgFCQY9RHopsjB83NKRp0ulbPxJ4EEQMg9PGoWxF4dE2o%3D&x="+str(p1+32)+"&y="+str(q1)) as response:
        data=await response.json()
        for ii in range(32):
            for jj in range(32):
                if data["materials"][ii][jj]=="FIRST" and p<=p1+32+ii<p+29 and q<=q1+jj<q+29:
                    return p1+32+ii-p
    async with session.get("https://prob16.geekgame.pku.edu.cn/api/state?token=19%3AMEUCIQC%2B0ZqalRVOpWGofI1Vj4wCoWXQxESMGdpvUl%2FWkYencQIgFCQY9RHopsjB83NKRp0ulbPxJ4EEQMg9PGoWxF4dE2o%3D&x="+str(p1)+"&y="+str(q1+32)) as response:
        data=await response.json()
        for ii in range(32):
            for jj in range(32):
                if data["materials"][ii][jj]=="FIRST" and p<=p1+ii<p+29 and q<=q1+jj+32<q+29:
                    return p1+ii-p
    async with session.get("https://prob16.geekgame.pku.edu.cn/api/state?token=19%3AMEUCIQC%2B0ZqalRVOpWGofI1Vj4wCoWXQxESMGdpvUl%2FWkYencQIgFCQY9RHopsjB83NKRp0ulbPxJ4EEQMg9PGoWxF4dE2o%3D&x="+str(p1+32)+"&y="+str(q1+32)) as response:
        data=await response.json()
        for ii in range(32):
            for jj in range(32):
                if data["materials"][ii][jj]=="FIRST" and p<=p1+ii+32<p+29 and q<=q1+jj+32<q+29:
                    return p1+32+ii-p

async def getdiff(i,session):
    return (await get(i,session))-(await get(i-1,session))

async def oddevenset(c,nums,step):
    async with aiohttp.ClientSession() as session:
        u=[await getdiff(i,session) for i in range(c,c+nums*step,step)]
        u1=u[0::2]
        u2=u[1::2]
    return len(set(u1)),len(set(u2))

def find(i,j):
    i1=i//32*32
    j1=j//32*32
    data=requests.get("https://prob16.geekgame.pku.edu.cn/api/state?token=19%3AMEUCIQC%2B0ZqalRVOpWGofI1Vj4wCoWXQxESMGdpvUl%2FWkYencQIgFCQY9RHopsjB83NKRp0ulbPxJ4EEQMg9PGoWxF4dE2o%3D&x="+str(i1)+"&y="+str(j1)).json()
    entity={}
    for ii in range(32):
        for jj in range(32):
            if data["materials"][ii][jj]!="AIR":
                entity[(i1+ii,j1+jj)]=data["materials"][ii][jj]
    return entity

def nonasync_get(i):
    p,q=getpq(i)
    p=p*29
    q=q*29
    p1=p//32*32
    q1=q//32*32
    data=requests.get("https://prob16.geekgame.pku.edu.cn/api/state?token=19%3AMEUCIQC%2B0ZqalRVOpWGofI1Vj4wCoWXQxESMGdpvUl%2FWkYencQIgFCQY9RHopsjB83NKRp0ulbPxJ4EEQMg9PGoWxF4dE2o%3D&x="+str(p1)+"&y="+str(q1)).json()
    for ii in range(32):
        for jj in range(32):
            if data["materials"][ii][jj]=="FIRST" and p<=p1+ii<p+29 and q<=q1+jj<q+29:
                return p1+ii-p
    data=requests.get("https://prob16.geekgame.pku.edu.cn/api/state?token=19%3AMEUCIQC%2B0ZqalRVOpWGofI1Vj4wCoWXQxESMGdpvUl%2FWkYencQIgFCQY9RHopsjB83NKRp0ulbPxJ4EEQMg9PGoWxF4dE2o%3D&x="+str(p1+32)+"&y="+str(q1)).json()
    for ii in range(32):
        for jj in range(32):
            if data["materials"][ii][jj]=="FIRST" and p<=p1+32+ii<p+29 and q<=q1+jj<q+29:
                return p1+32+ii-p
    data=requests.get("https://prob16.geekgame.pku.edu.cn/api/state?token=19%3AMEUCIQC%2B0ZqalRVOpWGofI1Vj4wCoWXQxESMGdpvUl%2FWkYencQIgFCQY9RHopsjB83NKRp0ulbPxJ4EEQMg9PGoWxF4dE2o%3D&x="+str(p1)+"&y="+str(q1+32)).json()
    for ii in range(32):
        for jj in range(32):
            if data["materials"][ii][jj]=="FIRST" and p<=p1+ii<p+29 and q<=q1+jj+32<q+29:
                return p1+ii-p
    data=requests.get("https://prob16.geekgame.pku.edu.cn/api/state?token=19%3AMEUCIQC%2B0ZqalRVOpWGofI1Vj4wCoWXQxESMGdpvUl%2FWkYencQIgFCQY9RHopsjB83NKRp0ulbPxJ4EEQMg9PGoWxF4dE2o%3D&x="+str(p1+32)+"&y="+str(q1+32)).json()
    for ii in range(32):
        for jj in range(32):
            if data["materials"][ii][jj]=="FIRST" and p<=p1+ii+32<p+29 and q<=q1+jj+32<q+29:
                return p1+32+ii-p

def xy53():
    data=requests.get("https://prob16.geekgame.pku.edu.cn/api/state?token=19%3AMEUCIQC%2B0ZqalRVOpWGofI1Vj4wCoWXQxESMGdpvUl%2FWkYencQIgFCQY9RHopsjB83NKRp0ulbPxJ4EEQMg9PGoWxF4dE2o%3D&x=0&y=0").json()
    for ii in range(32):
        for jj in range(32):
            if data["materials"][ii][jj]=="SECOND":
                return ii,jj
    data=requests.get("https://prob16.geekgame.pku.edu.cn/api/state?token=19%3AMEUCIQC%2B0ZqalRVOpWGofI1Vj4wCoWXQxESMGdpvUl%2FWkYencQIgFCQY9RHopsjB83NKRp0ulbPxJ4EEQMg9PGoWxF4dE2o%3D&x=32&y=0").json()
    for ii in range(21):
        for jj in range(32):
            if data["materials"][ii][jj]=="SECOND":
                return ii+32,jj
    data=requests.get("https://prob16.geekgame.pku.edu.cn/api/state?token=19%3AMEUCIQC%2B0ZqalRVOpWGofI1Vj4wCoWXQxESMGdpvUl%2FWkYencQIgFCQY9RHopsjB83NKRp0ulbPxJ4EEQMg9PGoWxF4dE2o%3D&x=0&y=32").json()
    for ii in range(32):
        for jj in range(21):
            if data["materials"][ii][jj]=="SECOND":
                return ii,jj+32
    data=requests.get("https://prob16.geekgame.pku.edu.cn/api/state?token=19%3AMEUCIQC%2B0ZqalRVOpWGofI1Vj4wCoWXQxESMGdpvUl%2FWkYencQIgFCQY9RHopsjB83NKRp0ulbPxJ4EEQMg9PGoWxF4dE2o%3D&x=32&y=32").json()
    for ii in range(21):
        for jj in range(21):
            if data["materials"][ii][jj]=="SECOND":
                return ii+32,jj+32

def xy29():
    data=requests.get("https://prob16.geekgame.pku.edu.cn/api/state?token=19%3AMEUCIQC%2B0ZqalRVOpWGofI1Vj4wCoWXQxESMGdpvUl%2FWkYencQIgFCQY9RHopsjB83NKRp0ulbPxJ4EEQMg9PGoWxF4dE2o%3D&x=0&y=0").json()
    for ii in range(29):
        for jj in range(29):
            if data["materials"][ii][jj]=="FIRST":
                return ii,jj

asyncio.set_event_loop_policy(asyncio.WindowsSelectorEventLoopPolicy())

c=0
step=1
nums=100
for i in range(32):
    u1,u2=asyncio.run(oddevenset(c,nums,step))
    print(c,u1,u2)
    if u2>u1:
        c=c+step
    step=step*2

zz=[nonasync_get(i) for i in range(100)]
x53,y53=xy53()
x29,y29=xy29()
for i in range(1,65537):
    k=i*(2**32)-c
    if [javarandom.Random(k+i).nextInt(29) for i in range(100)]==zz:
        k1=(85097958280569*k)%2**48
        u=javarandom.Random(k1*0x289223e1+0x4878857f)
        if u.nextInt(53)!=x53:
            continue
        if u.nextInt(53)!=y53:
            continue
        requests.post("https://prob16.geekgame.pku.edu.cn/api/break?token=19%3AMEUCIQC%2B0ZqalRVOpWGofI1Vj4wCoWXQxESMGdpvUl%2FWkYencQIgFCQY9RHopsjB83NKRp0ulbPxJ4EEQMg9PGoWxF4dE2o%3D&x="+str(x29)+"&y="+str(y29)).text
        requests.post("https://prob16.geekgame.pku.edu.cn/api/break?token=19%3AMEUCIQC%2B0ZqalRVOpWGofI1Vj4wCoWXQxESMGdpvUl%2FWkYencQIgFCQY9RHopsjB83NKRp0ulbPxJ4EEQMg9PGoWxF4dE2o%3D&x="+str(x53)+"&y="+str(y53)).text
        u=javarandom.Random(k1*0x3857dc53+0x6e3bd3dd)
        requests.post("https://prob16.geekgame.pku.edu.cn/api/break?token=19%3AMEUCIQC%2B0ZqalRVOpWGofI1Vj4wCoWXQxESMGdpvUl%2FWkYencQIgFCQY9RHopsjB83NKRp0ulbPxJ4EEQMg9PGoWxF4dE2o%3D&x="+str(u.nextInt(193))+"&y="+str(u.nextInt(193))).text
        u=javarandom.Random(k1*0x18b0a50b+0x27d58de9)
        requests.post("https://prob16.geekgame.pku.edu.cn/api/break?token=19%3AMEUCIQC%2B0ZqalRVOpWGofI1Vj4wCoWXQxESMGdpvUl%2FWkYencQIgFCQY9RHopsjB83NKRp0ulbPxJ4EEQMg9PGoWxF4dE2o%3D&x="+str(u.nextInt(1543))+"&y="+str(u.nextInt(1543))).text
        u=javarandom.Random(k1*0x1b7a1d5b+0x33f4f1a7)
        requests.post("https://prob16.geekgame.pku.edu.cn/api/break?token=19%3AMEUCIQC%2B0ZqalRVOpWGofI1Vj4wCoWXQxESMGdpvUl%2FWkYencQIgFCQY9RHopsjB83NKRp0ulbPxJ4EEQMg9PGoWxF4dE2o%3D&x="+str(u.nextInt(24593))+"&y="+str(u.nextInt(24593))).text
        u=javarandom.Random(k1*0x70517261+0x3ac463fb)
        requests.post("https://prob16.geekgame.pku.edu.cn/api/break?token=19%3AMEUCIQC%2B0ZqalRVOpWGofI1Vj4wCoWXQxESMGdpvUl%2FWkYencQIgFCQY9RHopsjB83NKRp0ulbPxJ4EEQMg9PGoWxF4dE2o%3D&x="+str(u.nextInt(393241))+"&y="+str(u.nextInt(393241))).text
        u=javarandom.Random(k1*0x5b07e3b5+0x6b85fa33)
        requests.post("https://prob16.geekgame.pku.edu.cn/api/break?token=19%3AMEUCIQC%2B0ZqalRVOpWGofI1Vj4wCoWXQxESMGdpvUl%2FWkYencQIgFCQY9RHopsjB83NKRp0ulbPxJ4EEQMg9PGoWxF4dE2o%3D&x="+str(u.nextInt(25165843))+"&y="+str(u.nextInt(25165843))).text
        
        