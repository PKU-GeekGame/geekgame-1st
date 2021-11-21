from pwn import *
import random
import minesweeper_util as u
import minesweeper as mnsw

W = 16
H = 16

def init_board():
    return [['x' for _ in range(W + 1)] for _ in range(H + 1)]


bound = [(0, 0)] + [(0, i) for i in range(1, W + 1)] + [(j, 0) for j in range(1, H + 1)]


def get_coord(cell):
    i, j = cell.split('-')
    return int(i), int(j)


def form_coord(i, j):
    return '%02d-%02d' % (i, j)


def get_str(board):
    return '\n'.join([''.join(board[i + 1][1:]) for i in range(H)])


def calc(board):
    state = u.generate_rules(u.Board(get_str(board)), 128)
#   print(state)
    
    extra_rules = []
    for i, j in bound:
        if board[i][j] == 'x':
            continue
        t = int(board[i][j])
        neighbor = []
        for i_ in range(i - 1, i + 2):
            for j_ in range(j - 1, j + 2):
                if i_ >= 1 and i_ <= H and j_ >= 1 and j_ <= W:
                    if board[i_][j_] == '*':
                        t = t - 1
                    elif board[i_][j_] == 'x':
                        neighbor.append(form_coord(i_, j_))
        if len(neighbor) > 0:
            extra_rules.append(mnsw.Rule(t, neighbor))
        
    solution = mnsw.solve(state[0] + extra_rules, 0.3)
    
    def get_cells(p):
        EPSILON = 1e-6
        return [k for k, v in solution.items() if abs(v - p) < EPSILON]
    
    mines, safe = get_cells(1.), get_cells(0.)
    
    for cell in mines:
        i, j = get_coord(cell)
        board[i][j] = '*'
        
    if len(safe) > 0:
        return get_coord(safe[0])
    else:
        for i, j in bound:
            i_, j_ = (i - 1) % H + 1, (j - 1) % W + 1
            if board[i][j] == 'x' and board[i_][j_] != 'x' and board[i_][j_] != '*':
                return i, j
                
        min_risk = min(solution.values())
        safest = get_cells(min_risk)
        
        moves = list(filter(None, safest))
        if len(moves) == 0:
            for i in range(1, H + 1):
                for j in range(1, W + 1):
                    move = form_coord(i, j)
                    if board[i][j] == 'x' and move not in solution.keys():
                        moves.append(move)
                        
        move, val = None, -1000
        for move_ in moves:
            i, j = get_coord(move_)
            val_ = (i - 8.5) ** 2 + (j - 8.5) ** 2
            if val_ > val:
                val = val_
                move = move_
        return get_coord(move)


    
def solve():
    conn = remote('prob09.geekgame.pku.edu.cn', 10009)
    conn.recvuntil(b'token:')
    conn.sendline(b'366:MEQCIHdFBn2RGvNI8+LjkanH7tTjHp3kvxY4io7Tq3sWc9ktAiBx5C3Axf8wvV/Qmei2AjcTqpFRulP6SGNCQkwsZ6ksxw==')
    conn.recvuntil(b')')
    conn.sendline(b'y')
    
    cnt = 0
    cnt_r = 0
    board = init_board()
    conn.recvline()
    while True:
        tmp = conn.recvuntil(b' ').strip().decode()
        if tmp.endswith('u'):
            print(tmp)
            break
#       special = [(16, 16), (15, 16), (16, 15)]
        special = [(16, 16), (1, 16), (16, 1)]
        if cnt < len(special):
            i, j = special[cnt]
        else:
            i, j = calc(board)
        
        guess = str(i - 1) + ' ' + str(j - 1)
        conn.sendline(guess.encode())
        cnt = cnt + 1
        resp = conn.recvline().strip().decode()
        if resp == 'BOOM!':
            cnt_r = cnt_r + 1
            if cnt_r >= 2000:
                conn.close()
                return False
            tmp = conn.recvuntil(b')').decode()
            conn.sendline(b'y')
            if cnt >= 60:
                print('Lose', cnt)
                print(get_str(board))
            conn.recvline()
            cnt = 0
            board = init_board()
        else:
            t = int(resp)
            board[i][j] = str(t)
    
    try:
        while True:
            print((conn.recvline().strip().decode()))
    except:
        return True
        
        
while True:
    t = solve()
    if t == True:
        break
        