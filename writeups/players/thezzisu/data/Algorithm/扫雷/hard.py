from pwn import *
from futils import *
from mt19937predictor import MT19937Predictor

r = remote('prob09.geekgame.pku.edu.cn', 10009)
r.sendline(b'TOKEN')
r.recvline()


def boom(first):
    r.sendline(b'n' if first else b'y')
    print(r.recvline())
    for i in range(16):
        for j in range(16):
            # print('Try '+str(i)+' '+str(j))
            # print((str(i)+' '+str(j)).encode())
            r.sendline((str(i)+' '+str(j)).encode())
            line = r.recvline()
            # print(line)
            ans = []
            if line == b'> BOOM!\n':
                for k in range(16):
                    line = r.recvline()
                    ans.append(line[:-1])
                r.recvline()
                return ans


pred = MT19937Predictor()

for i in range(78):
    bits = tr1(boom(i == 0))
    print("Time ", i)
    pred.setrandbits(bits, 16 * 16)

brd = show_board(fgb(pred.getrandbits(16 * 16)))
# bits = tr1(boom(False))
# print(bits)
# print()
r.sendline(b'y')
print(r.recvline())
for i in range(16):
    for j in range(16):
        if brd[i][j] != 42:
            r.sendline((str(i)+' '+str(j)).encode())
            line = r.recvline()

r.interactive()
