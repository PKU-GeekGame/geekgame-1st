import os
import sys
import random
import re
from subprocess import *
testDataPath = "test_mid.txt"
outPath = "test_mid_2.txt"
ansPath = "../msdata/easyres/res0.txt"
size = 9
INF = 100000
boomap = []
ansmap = []
souceName = "test.cpp"


def creatTestData(path):
    f = open(path, "w")
    f.write(str(size)+'\n')
    for i in range(size):
        for j in range(size):
            f.write("-2")
        f.write("\n")
    f.close()


def checkData(path):
    f = open(path, "r")
    inputData = f.readlines()
    inputData = inputData[1:]
    if len(inputData) < size:
        return False
    global boomap
    boomap = []
    y = 0
    for i in inputData:
        x = []
        for j in i:
            if (j == '\n'):
                continue
            if (ord(j) < ord("0") or ord(j) > ord('9')):
                if (j != '-'):
                    return False
            if (j == '-'):
                y = -INF
                continue
            else:
                if (y == -INF):
                    y = ord('0')-ord(j)
                else:
                    y = ord(j)-ord('0')
            x.append(y)
        boomap.append(x)
    f.close()
    return True  # check the map provide to test program


def readAnsMap(path):
    f = open(path, "r")
    inputData = f.readlines()
    global ansmap
    ansmap = []
    y = 0
    for i in inputData:
        x = []
        for j in i:
            if (j == '\n'):
                continue
            if (ord(j) < ord("0") or ord(j) > ord('9')):
                if (j != '-'):
                    return False
            if (j == '-'):
                y = -INF
                continue
            else:
                if (y == -INF):
                    y = ord('0')-ord(j)
                else:
                    y = ord(j)-ord('0')
            x.append(y)
        ansmap.append(x)
    f.close()
    return True


def checkBoom(path):
    f = open(path, "r")
    inputData = f.readlines()
    if len(inputData) != 1:
        return False
    inputData = inputData[0]
    mtch = r"\(\d+,\d+\)"
    try:
        out = re.match(mtch, inputData).group()
    except:
        return False
    out = eval(out)
    if (out[0] >= size or out[1] >= size):
        return False
    if (out[0] < 0 or out[1] < 0):
        return False
    if (boomap[out[0]][out[1]] != -2):
        return False
    return out  # check the program's output and get the position


def findZero(p, writePath):
    if (ansmap[p[0]][p[1]] == -1):
        return False
    import util
    q = util.Queue()
    q.push(p)
    while (not q.isEmpty()):
        p = q.pop()
        boomap[p[0]][p[1]] = ansmap[p[0]][p[1]]
        if (ansmap[p[0]][p[1]] != 0):
            continue
        x = p[0]+1
        y = p[1]
        if (x >= 0 and x < size and y >= 0 and y < size):
            if (boomap[x][y] == -2):
                q.push((x, y))
            boomap[x][y] = ansmap[x][y]
        x = p[0]-1
        y = p[1]
        if (x >= 0 and x < size and y >= 0 and y < size):
            if (boomap[x][y] == -2):
                q.push((x, y))
            boomap[x][y] = ansmap[x][y]
        x = p[0]
        y = p[1]+1
        if (x >= 0 and x < size and y >= 0 and y < size):
            if (boomap[x][y] == -2):
                q.push((x, y))
            boomap[x][y] = ansmap[x][y]
        x = p[0]
        y = p[1]-1
        if (x >= 0 and x < size and y >= 0 and y < size):
            if (boomap[x][y] == -2):
                q.push((x, y))
            boomap[x][y] = ansmap[x][y]
    print(boomap)
    f = open(writePath, "w")
    f.write(str(size)+'\n')
    for i in boomap:
        for j in i:
            f.write(str(j))
        f.write('\n')
    f.close
    return True


def checkIsEnd():
    for i in range(size):
        for j in range(size):
            if (boomap[i][j] == -2 and ansmap[i][j] != -1):
                return False
    return True


def Draw():
    import termcolor
    import cProfile
    print(" |", end='')
    for i in range(size):
        print(i, end=' ')
    print()
    print(" +", end='')
    for i in range(size):
        print("--", end='')
    print()
    for i in range(size):
        print(i, end='|')
        for j in range(size):
            if (boomap[i][j] == -2):
                termcolor.cprint("N", 'red',  end=' ')
            elif boomap[i][j] == 0:
                termcolor.cprint(" ", 'blue',  end=' ')
            else:
                termcolor.cprint(str(boomap[i][j]), 'blue',  end=' ')
        print()


if __name__ == '__main__':

    if (os.path.isfile(testDataPath)):  # 判断是否有test_mid.txt
        if (not checkData(testDataPath)):  # 有test_mid.txt,检查合法性，并构造map
            os.remove(testDataPath)       # 不合法
            creatTestData(testDataPath)   # 初始化
    else:
        creatTestData(testDataPath)  # 没有，初始化
        checkData(testDataPath)
    readAnsMap(ansPath)
    if (checkIsEnd()):
        print("Game is End and You win")
        exit(0)
    call("g++ "+souceName+" -o 1", shell=True)
    call("./1"+" < "+testDataPath+" > "+outPath, shell=True)
    out = checkBoom(outPath)
    if (out == False):
        Draw()
        print("The Point has been clicked")
        exit(0)

    if (not findZero(out, testDataPath)):
        Draw()
        print("Died,you find a boom")
        exit(0)
    Draw()
    print("----------------")
