import requests

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

def explore(x,y):
    return send_req('state',x,y,post=False)

def getpos(x,y):
    jt=explore(x,y)
    return jt['materials'][x%32][y%32]

balls=[("FIRST", 24, 0x746b, 0x48e5, 0x7d3bfc6f, 0x3f224939),
("SECOND", 48, 0x746b, 0x48e5, 0x7d3bfc6f, 0x3f224939),
("THIRD", 192, 0x746b, 0x48e5, 0x7d3bfc6f, 0x3f224939),
("FOURTH", 1536, 0x746b, 0x48e5, 0x7d3bfc6f, 0x3f224939),
("FIFTH", 24576, 0x746b, 0x48e5, 0x7d3bfc6f, 0x3f224939),
("SIXTH", 393216, 0x746b, 0x48e5, 0x7d3bfc6f, 0x3f224939),
("SEVENTH", 25165824, 0x746b, 0x48e5, 0x7d3bfc6f, 0x3f224939)]

ores=[]

for xi in range(-1,2):
    for yi in range(-1,2):
        pres=explore(xi*32,yi*32)
        nmap=pres['materials']
        for i in range(32):
            for j in range(32):
                if nmap[i][j]=='FIRST':
                    ores.append((xi*32+i,yi*32+j))

def toss(x,y):
    pick(x,y)
    for i in range(6):
        lip=balls[i+1][1]//balls[i][1]
        ltrux=x//balls[i][1]
        ltruy=y//balls[i][1]
        lremx=x%balls[i][1]
        lremy=y%balls[i][1]
        fnx=None
        fny=None
        for dx in range(lip):
            for dy in range(lip):
                if getpos(ltrux*balls[i+1][1]+lremx+dx*balls[i][1],ltruy*balls[i+1][1]+lremy+dy*balls[i][1])==balls[i+1][0]:
                    fnx=ltrux*balls[i+1][1]+lremx+dx*balls[i][1]
                    fny=ltruy*balls[i+1][1]+lremy+dy*balls[i][1]
                    prt=pick(fnx,fny)
                    print(prt)
                    break
            if fnx!=None:
                break
        x=fnx
        y=fny
                    

toss(ores[0][0],ores[0][1])

print(explore(0,0))