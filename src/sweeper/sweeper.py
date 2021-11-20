#!/usr/bin/env python3

import random
import signal

import functools
print = functools.partial(print, flush=True)

from mylib import show_flag


WIDTH = 16
HEIGHT = 16
ROUND_LIMIT = 2000


def gen_board():
    bits = random.getrandbits(WIDTH * HEIGHT)
    board = [[None] * WIDTH for _ in range(HEIGHT)]
    for i in range(HEIGHT):
        for j in range(WIDTH):
            x = (bits >> (i * WIDTH + j)) & 1
            board[i][j] = x
    return board


def check_win(board, marks):
    for i in range(HEIGHT):
        for j in range(WIDTH):
            if board[i][j] == 0 and not marks[i][j]:
                return False
    return True


def near_count(board, x, y):
    delta = [[1,0],[-1,0],[0,1],[0,-1],[1,1],[1,-1],[-1,1],[-1,-1]]
    count = 0
    for d in delta:
        tx, ty = x + d[0], y + d[1]
        if 0 <= tx < HEIGHT and 0 <= ty < WIDTH:
            count += board[tx][ty]
    return count


def show_board(board):
    for i in range(HEIGHT):
        for j in range(WIDTH):
            if board[i][j]:
                print("*", end='')
            else:
                print(near_count(board, i, j), end='')
        print()


def run(easy_mode):
    signal.alarm(600)
    count = 0

    while True:
        board = gen_board()
        count += 1
        marks = [[False] * WIDTH for _ in range(HEIGHT)]
        steps = 0

        print("New Game!")
        while not check_win(board, marks):
            x, y = input("> ").split()
            x, y = int(x), int(y)
            if board[x][y] != 0:
                if easy_mode and steps == 0:
                    while board[x][y] != 0:
                        board = gen_board()
                else:
                    print("BOOM!")
                    show_board(board)
                    break
            marks[x][y] = True
            print(near_count(board, x, y))
            steps += 1
        else: # win
            print("You win the game in %d steps!" % steps)
            show_flag(easy_mode)
            break
    
        if count >= ROUND_LIMIT:
            break

        print()
        t = input("try again? (y/n)")
        if t != 'y':
            print("bye")
            break
    

if __name__ == "__main__":
    print("Welcome to the minesweeper game!")

    t = input("easy mode? (y/n)")
    easy_mode = t == 'y'
    run(easy_mode)
