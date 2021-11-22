payload = '''
60 80 60 40 52 34 80 15 60 0f 57 60 00 80 fd 5b
50 60 40 51 61 00 e1 38 03 80 61 00 e1 83 39 81
81 01 60 40 52 60 60 81 10 15 60 31 57 60 00 80
fd 5b 81 01 90 80 80 51 90 60 20 01 90 92 91 90
80 51 90 60 20 01 90 92 91 90 80 51 90 60 20 01
90 92 91 90 50 50 50 82 73 ff ff ff ff ff ff ff
ff ff ff ff ff ff ff ff ff ff ff ff ff 16 63 7f
bf 5e 5a 83 83 60 40 51 83 63 ff ff ff ff 16 60
e0 1b 81 52 60 04 01 80 83 81 52 60 20 01 82 81
52 60 20 01 92 50 50 50 60 00 60 40 51 80 83 03
81 60 00 87 80 3b 15 80 15 60 b0 57 60 00 80 fd
5b 50 5a f1 15 80 15 60 c3 57 3d 60 00 80 3e 3d
60 00 fd 5b 50 50 50 50 33 73 ff ff ff ff ff ff
ff ff ff ff ff ff ff ff ff ff ff ff ff ff 16 ff
fe 00 00 00 00 00 00 00 00 00 00 00 00 a4 30 28
c7 02 c3 b1 19 c7 49 30 64 61 58 2b f6 47 fd 77
0a 15 ee a4 b2 55 1f 0c 96 d0 2a 5d 62 f8 4c ac
81 12 69 0d 68 c4 7b 16 81 4e 22 1b 8a 37 d6 c4
d3 29 3e de a6 61 63 5a ab cd 6d eb a6 15 ab 81
3a 76 10 c1 cf b9 ef b3 1c cc 52 24 c0 e4 b3 73
72
'''

payload = bytes.fromhex(payload.replace("\n", " "))

def bf(idx, target, key):
    return (target - (5 * idx + 7 * (key >> 4 * idx))) & 0xf

def calc(key, ans):
    flag = 0
    for idx in range(64):
        target = (ans >> 4 * idx) & 0xf
        flag |= bf(idx, target, key) << 4 * idx
    return flag

def check(key, ans):
    flag = calc(key, ans).to_bytes(32, "big")
    if b"flag" in flag or b"galf" in flag:
        print(flag)

for start in range(0, len(payload)):
    key = payload[start: start + 32]
    ans = payload[start + 32: start + 64]
    check(int.from_bytes(key, "big"), int.from_bytes(ans, "big"))