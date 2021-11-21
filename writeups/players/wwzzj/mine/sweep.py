from pwn import *
from time import sleep
sh = remote("prob09.geekgame.pku.edu.cn",10009)
r = sh.recv(numb = 2048, timeout = 2000)
# print(r)
token = "403:MEUCIQD3KO+wCR68OSEAc4JJYLR8Vtuwh//GTbDToFsQPITbuQIgXmMQ0znmP/DU2sKC0PX0ckbo779yoLYbKaO8pCbxgGo="
sh.sendline(token)
sleep(0.5)
r = sh.recv(numb = 2048, timeout = 2000)
# sh.sendline(" ")
# r = sh.recv(numb = 2048, timeout = 2000)
# print(r)
sh.sendline("y")
sleep(0.5)
r = sh.recv(numb = 2048, timeout = 2000)
print(r)

d = [[0, 1], [0, -1], [1, 0], [-1, 0], [1, 1], [1, -1], [-1, 1], [-1, -1]]

mm = [[{} for j in range(16)] for i in range(16)]
for i in range(16):
    for j in range(16):
        mm[i][j]['s'] = -2
acc =[[0] * 16 for i in range(16)]

known = 0
resM = 128

def get_min():

    mmin = 100000
    mminp = [0, 0]
    for i in range(0, 16):
        for j in range(0, 16):
            if mm[i][j]['s'] == -2:
                v = acc[i][j]
                if v == 0:
                    v = resM / (256 - known)
                if mmin > v:
                    mmin = v
                    mminp = [i, j]
    return mmin, mminp


def get_empty(x, y):
    e = []
    for i in d:
        tx = x + i[0]
        ty = y + i[0]
        if tx >= 0 and ty >= 0 and tx < 16 and ty < 16:
            if mm[tx][ty]['s'] == -2:
                e.append([tx, ty])
    return e

def update(x, y, val):
    global known
    known += 1
    for i in d:
        tx = x + i[0]
        ty = y + i[0]
        if tx >= 0 and ty >= 0 and tx < 16 and ty < 16:
            if mm[tx][ty]['s'] != -2:
                e = get_empty(tx, ty)
                for j in e:
                    acc[j[0]][j[1]] -= mm[tx][ty]['s'] / len(e)
    mm[x][y]['s'] = val
    for i in d:
        tx = x + i[0]
        ty = y + i[0]
        if tx >= 0 and ty >= 0 and tx < 16 and ty < 16:
            if mm[tx][ty]['s'] != -2:
                e = get_empty(tx, ty)
                for j in e:
                    acc[j[0]][j[1]] += mm[tx][ty]['s'] / len(e)
    e = get_empty(x, y)
    for j in e:
        acc[j[0]][j[1]] += mm[x][y]['s'] / len(e)


f = open("log.txt", "w+")

while True:
    mmin, dec = get_min()
    sh.sendline((str(dec[0]) + " " + str(dec[1])))
    sleep(0.5)
    r = sh.recv(numb = 2048, timeout = 2000)
    r = r.decode('utf-8')
    # print((str(dec[0]) + " " + str(dec[1])).encode('utf-8'))
    print("---------------------------", r, len(r))
    if len(r) > 20:
        print("*****************************")
        print(r, file=f)
        sh.sendline("y")
        sleep(0.5)
        r = sh.recv(numb = 2048, timeout = 2000)
        mm = [[{} for j in range(16)] for i in range(16)]
        for i in range(16):
            for j in range(16):
                mm[i][j]['s'] = -2
        acc =[[0] * 16 for i in range(16)]
        known = 0
        resM = 128
    else:
        update(dec[0], dec[1], int(r.split('\n')[0]))

