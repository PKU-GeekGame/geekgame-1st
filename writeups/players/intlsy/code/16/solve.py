import requests
import random
import sys

nameToId = { "AIR": 0, "FIRST": 1, "SECOND": 2, "THIRD": 3, "FOURTH": 4, "FIFTH": 5, "SIXTH": 6, "SEVENTH": 7 }
chunkSize = [-1,24,48,192,1536,24576,393216,25165824]

TOKEN = "PLACE YOUR TOKEN HERE"

def Break(x,y):
    params = {
        "token": TOKEN,
        "x": x,
        "y": y
    }
    t = requests.post(url="https://prob16.geekgame.pku.edu.cn/api/break?token="+TOKEN+"&x="+str(x)+"&y="+str(y),
            data = params)
    print(t.json())

def State(bx,by):
    # print(bx,by)
    t = requests.get(url="https://prob16.geekgame.pku.edu.cn/api/state?token="+TOKEN+"&x="+str(bx)+"&y="+str(by))
    try:
        return t.json()["materials"]
    except:
        print(t.text)
        sys.exit(0)
    
def Scan(x,y,targetId):
    bx = x // 32
    by = y // 32
    mapp = State(bx*32,by*32)
    result = []
    for i in range(32):
        for j in range(32):
            if nameToId[mapp[i][j]] == targetId:
                result.append( (bx*32+i,by*32+j) )
    return result

def Trace(id,lastX,lastY):
    nowChunkSize = chunkSize[id]
    lastChunkSize = chunkSize[id-1]
    for x in range(lastX,nowChunkSize,lastChunkSize):
        for y in range(lastY,nowChunkSize,lastChunkSize):
            result = Scan(x,y,id)
            if result:
                return result[0]
    print("WHAT THE FUCK?",id,lastX,lastY,nowChunkSize,lastChunkSize)
    return None
    

def Main():
    poses = []
    
    (lastX,lastY) = (0,0)
    for id in range(1,7+1):
        print("Finding ball #"+str(id))
        (lastX,lastY) = Trace(id,lastX,lastY) if id != 1 else Scan(0,0,1)[0]
        poses.append((lastX,lastY))
        
    print(poses)
    for pos in poses:
        Break(pos[0],pos[1])

Main()
