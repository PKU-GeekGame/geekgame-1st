from utils import *
import copy

testmap = [[1, 2, 2, 7, 7, 1, 1, 1, 1, 2, 2, 3, 9, 3, 1, 1, 5, 1, 2, 6, 1, 1, 1, 6, 1], [1, 1, 1, 1, 6, 2, 1, 1, 1, 1, 1, 1, 2, 2, 1, 8, 6, 1, 2, 1, 1, 1, 2, 2, 2], [1, 1, 1, 1, 2, 6, 1, 5, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 7, 1, 1, 1], [1, 1, 1, 2, 1, 1, 1, 2, 6, 1, 1, 6, 1, 1, 8, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1], [1, 1, 2, 7, 1, 7, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1], [1, 5, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 8, 1, 1, 5, 1, 1, 1, 1, 1, 1, 1, 1], [1, 1, 1, 1, 2, 1, 1, 6, 6, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 6, 2], [1, 1, 1, 1, 1, 1, 6, 1, 1, 1, 1, 1, 2, 2, 1, 1, 5, 1, 1, 1, 1, 1, 2, 6, 1], [1, 1, 1, 1, 8, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 7, 2, 1, 1, 1, 8, 1], [1, 1, 1, 6, 1, 1, 1, 1, 1, 8, 1, 1, 1, 2, 2, 6, 2, 1, 1, 1, 1, 1, 1, 1, 2], [2, 1, 8, 1, 1, 1, 1, 1, 1, 6, 1, 1, 1, 1, 2, 7, 2, 1, 1, 2, 1, 1, 1, 1, 1], [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 2, 1, 1, 1], [1, 8, 1, 8, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 6, 2, 1, 5, 1, 1, 1, 2, 1], [1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 6, 7, 1, 1, 6, 1, 1, 1, 1, 1], [1, 1, 5, 1, 7, 1, 6, 6, 2, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1], [1, 1, 1, 1, 1, 1, 1, 5, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1], [1, 1, 1, 1, 2, 1, 1, 6, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1], [1, 1, 1, 6, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1], [1, 1, 1, 7, 1, 1, 1, 8, 1, 1, 1, 1, 7, 1, 5, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1], [1, 5, 1, 1, 1, 8, 1, 1, 2, 8, 1, 2, 1, 2, 1, 7, 1, 1, 1, 6, 1, 1, 6, 1, 1], [1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1], [7, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 5, 1, 1, 1, 1, 1, 1, 2, 1, 1, 7], [1, 7, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2], [2, 7, 8, 8, 8, 1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 2, 8, 2, 1, 1, 1, 1, 1, 1], [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 6, 2, 1, 1, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1], [1, 1, 1, 1, 5, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 6, 1], [2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 1, 8, 1], [1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 7, 1], [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 2], [1, 1, 1, 1, 1, 7, 2, 1, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1], [1, 7, 1, 2, 1, 1, 6, 1, 1, 1, 1, 1, 1, 2, 1, 2, 2, 2, 1, 1, 2, 1, 1, 1, 1], [1, 1, 5, 1, 1, 1, 5, 1, 1, 1, 6, 1, 5, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 1], [1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 6, 1, 2, 1, 1, 1, 1, 7, 1, 1, 1, 1], [1, 1, 1, 1, 6, 1, 1, 1, 1, 5, 1, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 6, 1, 2, 1], [5, 7, 2, 6, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1], [1, 6, 1, 1, 8, 1, 1, 1, 7, 1, 2, 2, 1, 2, 2, 1, 1, 5, 1, 2, 6, 1, 1, 1, 2], [1, 1, 1, 5, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 6, 1, 1, 1, 1, 1, 1, 1, 1], [2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 6, 1, 1, 1, 2, 1, 1, 1, 1], [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1]]

DEBUG = False

global_cnt = 0
w_1 = 150
w_2 = -500
w_3 = -50

class Trans:
    left_jump = 0
    right_jump = 1
    left_dig = 2
    right_dig = 3

class Direction:
    left = 0
    right = 1

class Segment:
    def __init__(self, y, sx, ex):
        self.y = y
        self.sx = sx
        self.ex = ex
        self.money = 0
        self.heart = 0
        self.len = ex - sx + 1
        self.info = None

class Info:
    def __init__(self):
        self.money = 0
        self.heart = 0
        self.len = 0
        self.to_seg = None
        self.from_x = 0
        self.from_y = 0
        self.trans_type = 0

def is_chunk(map: list[list[int]], x: int, y: int) -> bool:
    return map[y][x] == Elem.chunk1 or map[y][x] == Elem.chunk2 or map[y][x] == Elem.chunk3

def is_transparent(map: list[list[int]], x: int, y: int) -> bool:
    return map[y][x] != Elem.dirt and map[y][x] != Elem.evildirt

def is_good_block(map: list[list[int]], x: int, y: int) -> bool:
    if map[y][x] == Elem.evildirt:
        return False
    if y == GY - 1:
        return True
    return not is_transparent(map, x, y + 1)

def money(map: list[list[int]], x: int, y: int) -> int:
    if map[y][x] == Elem.chunk1:
        return 100
    if map[y][x] == Elem.chunk2:
        return 200
    if map[y][x] == Elem.chunk3:
        return 300
    return 0

def heart(map: list[list[int]], x: int, y: int) -> int:
    if map[y][x] == Elem.heart:
        return 1
    return 0

def eat(map: list[list[int]], x: int, y: int) -> bool:
    return is_chunk(map, x, y) or map[y][x] == Elem.heart

def update(x: Info | None, y: Info) -> Info:
    if x == None:
        return y
    vx = x.money + x.heart * w_1 - x.len * w_2
    vy = y.money + y.heart * w_1 - x.len * w_2
    if vx > vy:
        return x
    return y


def get_seg(segs: list[Segment], x: int, y: int) -> Segment | None:
    for seg in segs[y]:
        if seg.sx <= x and seg.ex >= x:
            return seg
    return None

def jump(map: list[list[int]], segs: list[Segment], x: int, y: int, trans_type: int, from_x: int, from_y: int) -> Info:
    info = Info()
    info.trans_type = trans_type
    info.from_x = from_x
    info.from_y = from_y
    info.money = money(map, x, y)
    info.heart = heart(map, x, y)
    # seg = get_seg(segs, x, y)
    while y < GY - 1 and is_transparent(map, x, y + 1):
        y = y + 1
        info.money = info.money + money(map, x, y)
        info.heart = info.heart + heart(map, x, y)
    seg = get_seg(segs, x, y)
    info.to_seg = seg
    info.len = seg.len + 0 if seg.sx <= from_x <= seg.ex else min(abs(seg.sx - from_x), abs(seg.ex - from_x))
    return info

def player(map: list[list[int]]) -> list[int]:
    for y in range(GY):
        for x in range(GX):
            if map[y][x] == Elem.player:
                return [x, y]

def solve(_map: list[list[int]], w1, w2, w3 = False, w4 = False, w5 = False) -> list[Command]:
    global w_1
    global w_2
    global w_3
    w_1 = w1
    w_2 = w2
    w_3 = w3
    map = copy.deepcopy(_map)
    # map = _map
    if DEBUG:
        print(map)
    segments = [[] for y in range(GY)]
    for y in range(GY):
        start = -1
        for x in range(GX):
            if is_good_block(map, x, y):
                if start == -1:
                    start = x
            else:
                if start != -1:
                    segments[y].append(Segment(y, start, x - 1))
                    start = -1
        if start != -1:
            segments[y].append(Segment(y, start, GX - 1))
    for y in range(GY):
        for s in segments[y]:
            for x in range(s.sx, s.ex + 1):
                s.money += money(map, x, y)
                s.heart += heart(map, x, y)
            first, last = -1, -1
            for x in range(s.sx, s.ex + 1):
                if eat(map, x, s.y):
                    if first == -1:
                        first = x
                    last = x
            s.len = 0 if first == -1 else last - first + 1
    for _y in range(GY - 1):
        y = GY - _y - 2
        for s in segments[y]:
            best = None
            # Transfer to left
            if s.sx > 0 and is_transparent(map, s.sx - 1, y):
                best = update(best, jump(map, segments, s.sx - 1, y, Trans.left_jump, s.sx, y))
            # Transfer to right
            if s.ex < GX - 1 and is_transparent(map, s.ex + 1, y):
                best = update(best, jump(map, segments, s.ex + 1, y, Trans.right_jump, s.ex, y))
            # Transfer to bottom
            for x in range(s.sx, s.ex + 1):
                if x > 0 and map[y][x - 1] != Elem.evildirt and map[y + 1][x - 1] != Elem.evildirt:
                    if is_transparent(map, x - 1, y + 1):
                        best = update(best, jump(map, segments, x - 1, y + 1, Trans.left_dig, x, y))
                    else:
                        if y < GY - 2 and is_transparent(map, x - 1, y + 2):
                            best = update(best, jump(map, segments, x - 1, y + 2, Trans.left_dig, x, y))
                        else:
                            fseg = get_seg(segments, x - 1, y + 1)
                            if fseg != None:
                                info = Info()
                                info.money = fseg.money
                                info.heart = fseg.heart
                                info.to_seg = fseg
                                info.trans_type = Trans.left_dig
                                info.from_x = x
                                info.from_y = y
                                info.len = fseg.len
                                best = update(best, info)
                if x < GX - 1 and map[y][x + 1] != Elem.evildirt and map[y + 1][x + 1] != Elem.evildirt:
                    if is_transparent(map, x + 1, y + 1):
                        best = update(best, jump(map, segments, x + 1, y + 1, Trans.right_dig, x, y))
                    else:
                        if y < GY - 2 and is_transparent(map, x + 1, y + 2):
                            best = update(best, jump(map, segments, x + 1, y + 2, Trans.right_dig, x, y))
                        else:
                            fseg = get_seg(segments, x + 1, y + 1)
                            if fseg != None:
                                info = Info()
                                info.money = fseg.money
                                info.heart = fseg.heart
                                info.to_seg = fseg
                                info.trans_type = Trans.right_dig
                                info.from_x = x
                                info.from_y = y
                                info.len = fseg.len
                                best = update(best, info)
            if best != None:
                s.money = s.money + best.money
                s.heart = s.heart + best.heart
                s.len = s.len + best.len
                s.info = best
    player_x, player_y = player(map)
    player_dir = Direction.right
    init = get_seg(segments, player_x, player_y)
    if DEBUG:
        print(init.money)
    ptr = init
    cmd = []
    def addcmd(c: int):
        cmd.append(c)
        # cmd.append(Command.empty)
    while ptr != None:
        first, last = -1, -1
        fsec, lsec = -1, -1
        for x in range(ptr.sx, ptr.ex + 1):
            if eat(map, x, ptr.y):
                if first == -1:
                    first = x
                elif fsec == -1:
                    fsec = x
                lsec = last
                last = x
        if DEBUG:
            print('y=', ptr.y, 'sx=', ptr.sx, 'ex=', ptr.ex, 'first=', first, 'last=', last)
        assert player_y == ptr.y
        if ptr.info:
            if first == -1:
                first = last = ptr.info.from_x
            else:
                first = min(first, ptr.info.from_x)
                last = max(last, ptr.info.from_x)
            if fsec == -1:
                fsec = lsec = ptr.info.from_x
            else:
                fsec = min(fsec, ptr.info.from_x)
                lsec = max(lsec, ptr.info.from_x)
            if (last-first)-(lsec-fsec)>5 and w3:
                first=fsec
                last=lsec
            if DEBUG:
                print('from: ', ptr.info.from_x, ' ', ptr.info.from_y)
                print('current player: x=', player_x, ' y=', player_y)
            if w5:
                print('w5 mode')
                if player_dir == Direction.right:
                    while player_x < ptr.info.from_x:
                        if map[player_y][player_x + 1] == Elem.dirt:
                            addcmd(Command.next)
                            map[player_y][player_x + 1] = Elem.empty
                        addcmd(Command.right)
                        player_x = player_x + 1
                    if player_x != ptr.info.from_x:
                        addcmd(Command.left)
                        player_dir = Direction.left
                        while player_x > ptr.info.from_x:
                            if map[player_y][player_x - 1] == Elem.dirt:
                                addcmd(Command.next)
                                map[player_y][player_x - 1] = Elem.empty
                            addcmd(Command.left)
                            player_x = player_x - 1
                else:
                    while player_x > ptr.info.from_x:
                        if map[player_y][player_x - 1] == Elem.dirt:
                            addcmd(Command.next)
                            map[player_y][player_x - 1] = Elem.empty
                        addcmd(Command.left)
                        player_x = player_x - 1
                    if player_x != ptr.info.from_x:
                        addcmd(Command.right)
                        player_dir = Direction.right
                        while player_x < ptr.info.from_x:
                            if map[player_y][player_x + 1] == Elem.dirt:
                                addcmd(Command.next)
                                map[player_y][player_x + 1] = Elem.empty
                            addcmd(Command.right)
                            player_x = player_x + 1
            else:
                if player_dir == Direction.right:
                    while player_x < last:
                        if map[player_y][player_x + 1] == Elem.dirt:
                            addcmd(Command.next)
                            map[player_y][player_x + 1] = Elem.empty
                        addcmd(Command.right)
                        player_x = player_x + 1
                    tmp_money = 0
                    for i in range(first, player_x):
                        tmp_money += money(map, i, player_y)
                    tmp_money = tmp_money - (player_x - first) * 200
                    if w4 and tmp_money < 0:
                        if player_x != ptr.info.from_x:
                            addcmd(Command.left)
                            player_dir = Direction.left
                            while player_x > ptr.info.from_x:
                                if map[player_y][player_x - 1] == Elem.dirt:
                                    addcmd(Command.next)
                                    map[player_y][player_x - 1] = Elem.empty
                                addcmd(Command.left)
                                player_x = player_x - 1
                    else:
                        addcmd(Command.left)
                        player_dir = Direction.left
                        while player_x > first:
                            if map[player_y][player_x - 1] == Elem.dirt:
                                addcmd(Command.next)
                                map[player_y][player_x - 1] = Elem.empty
                            addcmd(Command.left)
                            player_x = player_x - 1
                        if player_x != ptr.info.from_x:
                            addcmd(Command.right)
                            player_dir = Direction.right
                            while player_x < ptr.info.from_x:
                                if map[player_y][player_x + 1] == Elem.dirt:
                                    addcmd(Command.next)
                                    map[player_y][player_x + 1] = Elem.empty
                                addcmd(Command.right)
                                player_x = player_x + 1
                else:
                    while player_x > first:
                        if map[player_y][player_x - 1] == Elem.dirt:
                            addcmd(Command.next)
                            map[player_y][player_x - 1] = Elem.empty
                        addcmd(Command.left)
                        player_x = player_x - 1
                    tmp_money = 0
                    for i in range(player_x + 1, last + 1):
                        tmp_money += money(map, i, player_y)
                    tmp_money = tmp_money - (last - player_x) * 200
                    if w4 and tmp_money < 0:
                        if player_x != ptr.info.from_x:
                            addcmd(Command.right)
                            player_dir = Direction.right
                            while player_x < ptr.info.from_x:
                                if map[player_y][player_x + 1] == Elem.dirt:
                                    addcmd(Command.next)
                                    map[player_y][player_x + 1] = Elem.empty
                                addcmd(Command.right)
                                player_x = player_x + 1
                    else:
                        addcmd(Command.right)
                        player_dir = Direction.right
                        while player_x < last:
                            if map[player_y][player_x + 1] == Elem.dirt:
                                addcmd(Command.next)
                                map[player_y][player_x + 1] = Elem.empty
                            addcmd(Command.right)
                            player_x = player_x + 1
                        if player_x != ptr.info.from_x:
                            addcmd(Command.left)
                            player_dir = Direction.left
                            while player_x > ptr.info.from_x:
                                if map[player_y][player_x - 1] == Elem.dirt:
                                    addcmd(Command.next)
                                    map[player_y][player_x - 1] = Elem.empty
                                addcmd(Command.left)
                                player_x = player_x - 1
            if DEBUG:
                print('current player: x=', player_x, ' y=', player_y)
            if ptr.info.trans_type == Trans.left_dig:
                if player_dir != Direction.left:
                    addcmd(Command.left)
                    player_dir = Direction.left
                addcmd(Command.down)
                addcmd(Command.left)
                player_x = player_x - 1
                map[player_y][player_x] = Elem.empty
                player_y = player_y + 1
                map[player_y][player_x] = Elem.empty
            if ptr.info.trans_type == Trans.left_jump:
                if player_dir != Direction.left:
                    addcmd(Command.left)
                    player_dir = Direction.left
                addcmd(Command.left)
                player_x = player_x - 1
            if ptr.info.trans_type == Trans.right_dig:
                if player_dir != Direction.right:
                    addcmd(Command.right)
                    player_dir = Direction.right
                addcmd(Command.down)
                addcmd(Command.right)
                player_x = player_x + 1
                map[player_y][player_x] = Elem.empty
                player_y = player_y + 1
                map[player_y][player_x] = Elem.empty
            if ptr.info.trans_type == Trans.right_jump:
                if player_dir != Direction.right:
                    addcmd(Command.right)
                    player_dir = Direction.right
                addcmd(Command.right)
                player_x = player_x + 1
            while player_y < GY - 1 and is_transparent(map, player_x, player_y + 1):
                player_y = player_y + 1
            if DEBUG:
                print('trans_type:', ptr.info.trans_type)
                print('to: ', player_x, ' ', player_y)
        ptr = None if ptr.info == None else ptr.info.to_seg
    if DEBUG:
        print('CMD:')
        print(cmd, flush=True)
    return cmd

def better_one(l, r):
    return l if len(l) < len(r) else r

def better_solve(map: list[list[int]]):
    global global_cnt
    global_cnt = global_cnt + 1
    solu0 = solve(map, 150, -100, False, True, True)
    # if global_cnt < 5:
    #     solu0 = solve(map, 50, -2000, True, True, True)
    # if global_cnt < 10:
    #     solu0 = solve(map, 50, -1000, 0)
    return solu0


if __name__ == '__main__':
    print(solve(testmap))
    import debug
    debug.draw(testmap)
    debug.start()