from pwn import *
import sys, os

sys.setrecursionlimit(10000)

token = os.getenv("TOKEN")
# conn = process(["python3", "sweeper.py"])
conn = remote("prob09.geekgame.pku.edu.cn", 10009)
conn.recvuntil("token:")
conn.sendline(token)

def recv():
    global conn
    conn.recvuntil("easy mode? (y/n)")
    conn.sendline("y")
    conn.recvuntil(">")
    while True:
        try:
            for i in range(16):
                for j in range(16):
                    conn.sendline("%d %d" % (i, j))
                    data = conn.recvline()
                    if b"BOOM!" in data:
                        w = ""
                        r = []
                        for i in range(16):
                            data = conn.recvline().strip(b"\n")
                            data = "".join(["1" if x == 42 else "0" for x in data])
                            w += data
                        for i in range(8):
                            st = 32 * i
                            o = w[st: st + 32]
                            o = o[::-1]
                            val = int(o, 2)
                            r.append(val)
                        conn.recvuntil("try again? (y/n)")
                        conn.sendline("y")
                        conn.recvuntil(">")
                        yield r
                        raise RuntimeError("break")
                    else:
                        conn.recvuntil(">")
        except RuntimeError as e:
            if str(e) != "break":
                raise e

def _int32(x):
    return int(0xFFFFFFFF & x)

class MT19937:
    def __init__(self, seed=0):
        self.mt = [0] * 624
        self.mt[0] = seed
        self.mti = 0
        for i in range(1, 624):
            self.mt[i] = _int32(1812433253 * (self.mt[i - 1] ^ self.mt[i - 1] >> 30) + i)
    def getstate(self, op=False):
        if self.mti == 0 and op==False:
            self.twist()
        y = self.mt[self.mti]
        y = y ^ y >> 11
        y = y ^ y << 7 & 2636928640
        y = y ^ y << 15 & 4022730752
        y = y ^ y >> 18
        self.mti = (self.mti + 1) % 624
        return _int32(y)
    def twist(self):
        for i in range(0, 624):
            y = _int32((self.mt[i] & 0x80000000) + (self.mt[(i + 1) % 624] & 0x7fffffff))
            self.mt[i] = (y >> 1) ^ self.mt[(i + 397) % 624]
            if y % 2 != 0:
                self.mt[i] = self.mt[i] ^ 0x9908b0df
    def inverse_right(self, res, shift, mask=0xffffffff, bits=32):
        tmp = res
        for i in range(bits // shift):
            tmp = res ^ tmp >> shift & mask
        return tmp
    def inverse_left(self, res, shift, mask=0xffffffff, bits=32):
        tmp = res
        for i in range(bits // shift):
            tmp = res ^ tmp << shift & mask
        return tmp
    def extract_number(self, y):
        y = y ^ y >> 11
        y = y ^ y << 7 & 2636928640
        y = y ^ y << 15 & 4022730752
        y = y ^ y >> 18
        return y & 0xffffffff
    def recover(self, y):
        y = self.inverse_right(y, 18)
        y = self.inverse_left(y, 15, 4022730752)
        y = self.inverse_left(y, 7, 2636928640)
        y = self.inverse_right(y, 11)
        return y & 0xffffffff
    def setstate(self, s):
        if len(s) != 624:
            raise ValueError("The length of prediction must be 624!")
        for i in range(624):
            self.mt[i] = self.recover(s[i])
        self.mti = 0
    def predict(self, s):
        self.setstate(s)
        self.twist()
        return self.getstate(True)
    def invtwist(self):
        high = 0x80000000
        low = 0x7fffffff
        mask = 0x9908b0df
        for i in range(623,-1,-1):
            tmp = self.mt[i] ^ self.mt[(i + 397)%624]
            if tmp & high == high:
                tmp ^= mask
                tmp <<= 1
                tmp |= 1
            else:
                tmp <<=1
            res = tmp & high
            tmp = self.mt[i - 1] ^ self.mt[(i + 396)%624]
            if tmp & high == high:
                tmp ^= mask
                tmp <<= 1
                tmp |= 1
            else:
                tmp <<=1
            res |= tmp & low
            self.mt[i] = res

def ABC_D(A, B, C):
    '''
    a[idx], a[idx + 1], a[idx + 397] => nxt_a[idx]
    '''
    a = mt.recover(A)
    b = mt.recover(B)
    c = mt.recover(C)
    y = _int32((a & 0x80000000) + (b & 0x7fffffff))
    pred = (y >> 1) ^ c
    if y % 2 != 0:
        pred ^= 0x9908b0df
    pred = pred ^ pred >> 11
    pred = pred ^ pred << 7 & 2636928640
    pred = pred ^ pred << 15 & 4022730752
    pred = pred ^ pred >> 18
    return pred

def ABD_C(A, B, D):
    '''
    a[idx], a[idx + 1], nxt_a[idx] => a[idx + 397]
    '''
    a = mt.recover(A)
    b = mt.recover(B)
    y = _int32((a & 0x80000000) + (b & 0x7fffffff))
    pred = D
    pred = mt.inverse_right(pred, 18)
    pred = mt.inverse_left(pred, 15, 4022730752)
    pred = mt.inverse_left(pred, 7, 2636928640)
    pred = mt.inverse_right(pred, 11)
    if y % 2 != 0:
        pred ^= 0x9908b0df
    c = pred ^ (y >> 1)
    c = c ^ c >> 11
    c = c ^ c << 7 & 2636928640
    c = c ^ c << 15 & 4022730752
    c = c ^ c >> 18
    return c

mt = MT19937(4)
gen = recv()

vector = []

for i in range(1950):
    vector.append(next(gen))

allSet = {}
vis = {}
dis = {}
edge = {}

def setdis(x, y, d):
    if x not in edge:
        edge[x] = []
    if y not in edge:
        edge[y] = []
    edge[x].append((y, d))
    edge[y].append((x, -d))

def rule0(x, y):
    setdis(x, y, 49)

def rule1(x, y):
    setdis(x, y, 50)

def rule2(x, y):
    setdis(x, y, 78)

for idx0, arr in enumerate(vector):
    for idx1, j in enumerate(arr):
        assert j not in allSet
        allSet[j] = (idx0, idx1)

for i in range(len(vector)):
    A = vector[i][0]
    B = vector[i][1]
    for j in range(i + 1, min(i + 79, len(vector))):
        if ABC_D(vector[i][0], vector[i][1], vector[j][5]) in allSet:
            if ABC_D(vector[i][1], vector[i][2], vector[j][6]) in allSet:
                if ABC_D(vector[i][2], vector[i][3], vector[j][7]) in allSet:
                    rule0(i, j)
                    rule2(i, allSet[ABC_D(vector[i][0], vector[i][1], vector[j][5])][0])
        if ABC_D(vector[i][3], vector[i][4], vector[j][0]) in allSet:
            if ABC_D(vector[i][4], vector[i][5], vector[j][1]) in allSet:
                if ABC_D(vector[i][5], vector[i][6], vector[j][2]) in allSet:
                    if ABC_D(vector[i][6], vector[i][7], vector[j][3]) in allSet:
                        rule1(i, j)
                        rule2(i, allSet[ABC_D(vector[i][3], vector[i][4], vector[j][0])][0])

extra = []

def dfs(idx, color):
    if idx in vis:
        if dis[idx] != color:
            print(idx, dis[idx], color)
        assert dis[idx] == color
        return
    extra.append(idx)
    vis[idx] = 1
    dis[idx] = color
    if idx not in edge:
        return
    for out in edge[idx]:
        y, delta = out
        dfs(y, color + delta)

def output(arr):
    print("rolling")
    buffer = arr
    print(buffer[-8:])
    roll_cnt = 0
    first = True
    while buffer[-8:] != vector[-1]:
        roll_cnt += 1
        if roll_cnt % 100 == 0:
            print("roll_cnt", roll_cnt)
        if first:
            w = mt.predict(buffer[-624: ])
        else:
            w = mt.getstate()
        buffer.append(w)
    
    ans = []

    for i in range(8):
        w = mt.predict(buffer[-624: ])
        for j in range(32):
            ans.append(w >> j & 1)
        buffer.append(w)
    
    for i in range(16):
        for j in range(16):
            if ans[i * 16 + j] == 0:
                conn.sendline("%d %d" % (i, j))

    print(conn.recvall().decode("utf-8").split("\n")[-3:])
    quit(0)


def collect_ans(full):
    state = {}
    for idx in full:
        color = dis[idx]
        for i in range(8):
            state[8 * color + i] = vector[idx][i]
    for loop in range(20):
        keys = list(sorted(state.keys()))
        for i in keys:
            if i in state and i + 1 in state and i + 397 in state:
                tmp = ABC_D(state[i], state[i + 1], state[i + 397])
                if i + 624 in state:
                    assert state[i + 624] == tmp
                state[i + 624] = tmp
            if i in state and i + 1 in state and i + 624 in state:
                tmp = ABD_C(state[i], state[i + 1], state[i + 624])
                if i + 397 in state:
                    assert state[i + 397] == tmp
                state[i + 397] = tmp
    print(list(state.keys())[: 50])
    cnt = 0; lst = 0
    for data in sorted(state.keys()):
        if data == lst + 1:
            cnt += 1
        else:
            cnt = 1
        lst = data
        if cnt == 624:
            for idx in full:
                print(idx, dis[idx])
            print("=>", data)
            hope = []
            for i in range(data - 623, data + 1):
                hope.append(state[i])
            output(hope)

for loop in range(10):
    print("loop %d" % loop)
    dis = {}; col = {}
    vis = {}; sto = []
    for i in range(len(vector)):
        if i not in vis:
            extra = []
            dfs(i, 0)
            extra.sort()
            for idx in range(len(extra) - 1):
                delta = dis[extra[idx + 1]] - dis[extra[idx]]
                if delta > 10:
                    continue
                if delta == extra[idx + 1] - extra[idx]:
                    for k in range(delta - 1):
                        sto.append(extra[idx] + k)
            disSet = set()
            for data in extra:
                disSet.add((dis[data] % 78 + 78) % 78)
            if len(disSet) > 10:
                print(len(disSet))
            if len(disSet) == 78:
                if loop == 9:
                    collect_ans(extra)
    for x in sto:
        setdis(x, x + 1, 1)