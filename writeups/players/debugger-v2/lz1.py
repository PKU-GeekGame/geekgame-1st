import aiohttp, asyncio, requests
async def get(i,j,session):
    i1=i//32*32
    j1=j//32*32
    async with session.get(url="https://prob16.geekgame.pku.edu.cn/api/state?token=19%3AMEUCIQC%2B0ZqalRVOpWGofI1Vj4wCoWXQxESMGdpvUl%2FWkYencQIgFCQY9RHopsjB83NKRp0ulbPxJ4EEQMg9PGoWxF4dE2o%3D&x="+str(i1)+"&y="+str(j1)) as response:
        data=await response.json()
        if data["materials"][i-i1][j-j1]!="AIR":
            print(str(i)+","+str(j)+" "+data["materials"][i-i1][j-j1])


async def search(i,j,s,n):
    async with aiohttp.ClientSession() as session:
        for p in range(n):
            for q in range(n):
                await get(i+p*s,j+q*s,session)

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
