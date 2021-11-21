import random, time, socket
from randcrack import RandCrack

WIDTH = 16
HEIGHT = 16
TOKEN = "PLACE YOUR TOKEN HERE"

def ReadBoard(inputs):
    board = [[None] * WIDTH for _ in range(HEIGHT)]
    for i in range(HEIGHT):
        for j in range(WIDTH):
            board[i][j] = 1 if inputs[i][j] == '*' else 0
    return board

def GetBits(board):
    bits = 0
    for i in range(HEIGHT):
        for j in range(WIDTH):
            bits |= board[i][j] << (i*WIDTH+j)

def GetNums(board):
    nums = []
    count = 0
    curNum = 0
    for i in range(HEIGHT-1,-1,-1):
        for j in range(WIDTH-1,-1,-1):
            curNum = (curNum<<1) | board[i][j]
            count += 1
            if count == 32:
                nums.append(curNum)
                count = 0
                curNum = 0
    nums.reverse()
    return nums


def Main():
    c = socket.socket()
    def RecvStr():
        data = c.recv(2048).decode()
        print("[<--",data)
        return data
    def SendStr(s):
        print("[==>",s)
        c.send((s+'\n').encode())
        time.sleep(0.07)
        
    c.connect(("prob09.geekgame.pku.edu.cn",10009))
    RecvStr()
    SendStr(TOKEN)
    RecvStr()   # welcome to the minesweeper game\n easy mode?
    
    SendStr('n')
    
    nums = []
    while len(nums) < 624:
        s = RecvStr()   # New Game!\n >
        assert 'New Game!' in s
        while True:
            SendStr('%d %d'%(random.randint(0,15),random.randint(0,15)))
            s = RecvStr()
            if 'BOOM!' in s:
                mapp = s
                break
            else:
                continue
        
        board = ReadBoard(mapp.split('\n')[1:])
        nowNums = GetNums(board)
        nums.extend(nowNums)
        print(nowNums)
        
        SendStr('y')
    
    rc = RandCrack()
    for x in nums[:624]:
        rc.submit(x)
    
    newBoardBits = rc.predict_getrandbits(WIDTH*HEIGHT)
    def GenBoard(bits):
        board = [[None] * WIDTH for _ in range(HEIGHT)]
        for i in range(HEIGHT):
            for j in range(WIDTH):
                x = (bits >> (i * WIDTH + j)) & 1
                board[i][j] = x
        return board
    board = GenBoard(newBoardBits)
    print(board)
    
    s = RecvStr()   # New Game!
    assert 'New Game!' in s
    for i in range(HEIGHT):
        for j in range(WIDTH):
            if board[i][j] == 1: continue
            SendStr('%d %d'%(i,j))
            s = RecvStr()
            print(s)
            if 'BOOM!' in s:
                print("FUCKKKKKK???????")
                break
    s = RecvStr()
    print(s)
            
Main()
