from pwn import *
import random

#context.log_level = "debug"


def new_session():
    r = process(["python3", "sweeper.py"])
    return r


def inv_rshift(y, shift):
    x = 0
    while y != x ^ (x >> shift):
        x = y ^ (x >> shift)
    return x


def inv_lshift(y, shift, mask):
    x = 0
    while y != x ^ ((x << shift) & mask):
        x = y ^ ((x << shift) & mask)
    return x


def inv_state(value):
    value = inv_rshift(value, 18)
    value = inv_lshift(value, 15, 0xefc60000)
    value = inv_lshift(value, 7, 0x9d2c5680)
    value = inv_rshift(value, 11)
    return value


def extract_value(state):
    state ^= state >> 11
    state ^= (state << 7) & 0x9d2c5680
    state ^= (state << 15) & 0xefc60000
    state ^= state >> 18
    return state


def next_state(s0, s1, s397):
    y = (s0 & 0x80000000) | (s1 & 0x7FFFFFFF)
    x = (y >> 1) ^ s397
    if (y & 1) == 1:
        x ^= 0x9908b0df
    return x


def recv_board(session):
    board = 0
    for i in range(16):
        line = session.recvline().strip().decode()
        for j in range(16):
            board |= int(line[j] == "*") << (i * 16 + j)
    return board


def solve1():
    session = new_session()
    session.recvuntil("easy mode")
    session.sendline('n')

    states = []
    while len(states) < 624:
        session.recvuntil("New Game!")
        while True:
            x = random.randint(0, 15)
            y = random.randint(0, 15)
            session.recvuntil("> ")
            session.sendline("%d %d" % (x, y))
            resp = session.recvline()
            if b"BOOM!" in resp:
                break

        board = recv_board(session)
        for i in range(8):
            state = (board >> (32 * i)) & 0xFFFFFFFF
            state = inv_state(state)
            states.append(state)

        session.recvuntil("again?")
        session.sendline("y")

    board = 0
    for i in range(8):
        state = next_state(states[0], states[1], states[397])
        board |= extract_value(state) << (32 * i)
        states = states[1:] + [state]

    session.recvuntil("New Game!")
    for i in range(16):
        for j in range(16):
            if ((board >> (16 * i + j)) & 1) == 0:
                session.recvuntil("> ")
                session.sendline("%d %d" % (i, j))
                resp = session.recvline()
                assert not b"BOOM!" in resp

    print(session.recvall().decode())


def predict_board(states):
    front9 = states[0] + [states[1][0]]
    # ([8][b8])*13 (-208) [b8] (-216) [8] (-224) [8] (-232)
    mid8 = states[-15][-3:] + states[-14][:5]
    board = 0
    for i in range(8):
        state = next_state(front9[i], front9[i+1], mid8[i])
        board |= extract_value(state) << (32 * i)
    return board


def solve2():
    session = new_session()
    session.recvuntil("easy mode")
    session.sendline('y')

    states = [[0] * 8 for _ in range(40)]

    cnt = 0
    while True:
        cnt += 1
        print("run", cnt)

        board = predict_board(states)
        positions = [(i, j) for i in range(16) for j in range(16)
                     if ((board >> (16 * i + j)) & 1) == 0]
        for pos in positions:
            session.recvuntil("> ")
            session.sendline("%d %d" % pos)
            resp = session.recvline()
            if b"BOOM!" in resp:
                break
        else: # success
            break

        board = recv_board(session)
        state = []
        for i in range(8):
            s = (board >> (32 * i)) & 0xFFFFFFFF
            s = inv_state(s)
            state.append(s)
        states = states[1:] + [state]

        session.recvuntil("again?")
        session.sendline("y")

    print(session.recvall().decode())


solve1()
solve2()
