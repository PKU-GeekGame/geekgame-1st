from pwn import *
from mt19937predictor import MT19937Predictor

conn = remote('prob09.geekgame.pku.edu.cn', 10009)
conn.recvuntil(b'token:')
conn.sendline(b'366:MEQCIHdFBn2RGvNI8+LjkanH7tTjHp3kvxY4io7Tq3sWc9ktAiBx5C3Axf8wvV/Qmei2AjcTqpFRulP6SGNCQkwsZ6ksxw==')
conn.recvuntil(b')')
conn.sendline(b'n')

cnt = 0
index = 0
predictor = MT19937Predictor()
while cnt <= 100:
    conn.recvuntil(b'> ')
    guess = str(index // 16) + ' ' + str(index % 16)
    conn.sendline(guess.encode())
    resp = conn.recvline().strip()
    if resp == b'BOOM!':
        s = ''
        for _ in range(16):
            s += conn.recvline().strip().decode()
        bits = 0
        for i in range(256):
            if s[i] == '*':
                bits += (1 << i)
        predictor.setrandbits(bits, 256)
        conn.recvuntil(b')')
        conn.sendline(b'y')
        cnt = cnt + 1
    else:
        index = (index + 1) % 256


        
bits = predictor.getrandbits(256)
ans = []
for i in range(16):
    for j in range(16):
        x = (bits >> (i * 16 + j)) & 1
        if x == 0:
            ans.append((i, j))
for i, j in ans:
    conn.recvuntil(b'> ')
    guess = str(i) + ' ' + str(j)
    conn.sendline(guess.encode())
while True:
    print(conn.recvline().strip().decode())
        