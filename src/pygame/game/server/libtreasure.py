#coding=utf-8
import random
import math
import copy
import struct
import time

from flag import getflag, checktoken
import logger

GX = 25
GY = 39
INIT_LEVEL = 1
ELEM_PROB = .1
EVIL_PROB = .12
HEART_LIFE = 25
BURN_LIFE = 20
RESTORE_TIME = 10
GOAL_OF_LEVEL = lambda level: 300+int(level**1.5)*100
GOOD_TIME_S = 100

class GameOver(Exception):
    def __str__(self): return 'Game Over'

class YouWin(Exception):
    def __str__(self): return 'You Win'

class _FireStopped(Exception):
    pass

class CheckerMixin:
    @classmethod
    def _check(cls, value):
        for k, v in cls.__dict__.items():
            if not k.startswith('_') and v==value:
                return v
        raise RuntimeError('bad value %s for %s'%(value, cls))

class Elem(CheckerMixin):
    dirt = 1
    evildirt = 2
    empty = 3
    fire = 4
    heart = 5
    chunk1 = 6
    chunk2 = 7
    chunk3 = 8
    chunks = [6, 7, 8]
    player = 9

class Command(CheckerMixin):
    left = 1
    right = 2
    next = 3
    down = 4
    empty = 5

class Sync(CheckerMixin):
    sync_level = 1
    sync_status = 2
    sync_map = 3
    init_level = 4
    show_hud = 5
    tick_routine = 6

SALARY = {Elem.chunk1: 100, Elem.chunk2: 200, Elem.chunk3: 300}

class Fire:
    def __init__(self, game, y, x):
        self.left = False
        self.y = y
        self.x = x
        self.game = game

    def tick(self):
        _t = {Elem.dirt, Elem.evildirt, Elem.fire}

        if self.y<GY-1 and self.game.g[self.y+1][self.x]==Elem.empty: #go down
            self.game.g[self.y+1][self.x] = Elem.fire
            self.game.g[self.y][self.x] = Elem.empty
            self.y += 1
        elif self.y<GY-1 and self.game.g[self.y+1][self.x] in Elem.chunks+[Elem.heart]: #burn down
            self.game.new_fire(self.y+1,self.x)
        elif self.y<GY-1 and self.game.g[self.y+1][self.x]==Elem.player: #beat down
            if self.game.player.hurt(BURN_LIFE):
                 raise _FireStopped()
        elif 0<self.x<GX-1 and self.game.g[self.y][self.x+1] in _t and self.game.g[self.y][self.x-1] in _t: #do nothing A
            return
        elif (self.x==0 and self.game.g[self.y][1] in _t) or (self.x==GX-1 and self.game.g[self.y][GX-2] in _t): #do nothing B
            return
        else:
            if self.x==0:
                self.left = False
            if self.x==GX-1:
                self.left = True
            nextx = self.x-1 if self.left else self.x+1
            if self.game.g[self.y][nextx] in _t:
                self.left = not self.left
                nextx = self.x-1 if self.left else self.x+1

            if self.game.g[self.y][nextx]==Elem.empty: #go next
                self.game.g[self.y][nextx] = Elem.fire
                self.game.g[self.y][self.x] = Elem.empty
                self.x = nextx
            elif self.game.g[self.y][nextx] in Elem.chunks+[Elem.heart]: #burn next
                self.game.new_fire(self.y,nextx)
            elif self.game.g[self.y][nextx]==Elem.fire: #ourselves, just keep calm
                pass
            elif self.game.g[self.y][nextx]==Elem.player: #beat next
                if self.game.player.hurt(BURN_LIFE):
                    raise _FireStopped()
            else:
                raise RuntimeError('bad next elem: %s'%self.game.g[self.y][nextx])


class Player:
    def __init__(self, game, y, x):
        self.life = 100
        self.life_restore = 0
        self.life_waiting = False
        self.left = False
        self.command = Command.empty
        self.game = game
        self.y = y
        self.x = x

    def _move(self, y, x):
        def go():
            self.game.g[self.y][self.x] = Elem.empty
            self.game.g[y][x] = Elem.player
            self.y = y
            self.x = x

        if not (0<=x<GX and 0<=y<GY):
            return False
        if self.game.g[y][x]==Elem.empty:
            return go() or True
        elif self.game.g[y][x]==Elem.heart:
            self.life = min(self.life+HEART_LIFE,100)
            return go() or True
        elif self.game.g[y][x] in Elem.chunks:
            self.game.cur += SALARY[self.game.g[y][x]]
            if self.game.cur>=self.game.goal:
                raise YouWin()
            return go() or True
        elif self.game.g[y][x] == Elem.fire:
            for fire in self.game.fires:
                if fire.y==y and fire.x==x:
                    if self.hurt(BURN_LIFE):
                        self.game.g[y][x] = Elem.empty
                        self.game.fires.remove(fire)
                        return self._move(y, x)
                    else:
                        return False
            else:
                raise RuntimeError('cant find right fire')

    def _digg(self, y, x):
        if not (0<=x<GX and 0<=y<GY):
            return
        if self.game.g[y][x]==Elem.dirt:
            self.game.g[y][x] = Elem.empty
        elif self.game.g[y][x]==Elem.evildirt:
            self.game.new_fire(y,x)

    def tick(self):
        if self.life_restore:
            self.life_restore -= 1
        if not self.life_waiting:
            self.life -= 1
        self.life_waiting = not self.life_waiting
        if self.life<=0:
            raise GameOver()

        if not self._move(self.y+1, self.x):
            if self.command==Command.left:
                if self.left:
                    self._move(self.y, self.x-1)
                self.left = True
            elif self.command==Command.right:
                if not self.left:
                    self._move(self.y, self.x+1)
                self.left = False
            elif self.command==Command.next:
                self._digg(self.y, self.x+(-1 if self.left else 1))
            elif self.command==Command.down:
                self._digg(self.y, self.x+(-1 if self.left else 1))
                self._digg(self.y+1, self.x+(-1 if self.left else 1))
            self.command = Command.empty

    def hurt(self, value):
        if self.life_restore:
            return False
        else:
            self.life -= value
            self.life_restore = RESTORE_TIME
            if self.life<=0:
                raise GameOver()
            return True

class Game:
    def __init__(self, socket, token):
        self.g = None
        self.level = -1
        self.cur = 0
        self.goal = 0
        self.fires = set()
        self.player = Player(self, 0, 0)
        self.fire_ticked = False

        self.socket = socket
        self.synced_g = None
        self.token = token
        self.uid = checktoken(token)

    def wait_command(self):
        b = self.socket.recv(1)
        return Command._check(struct.unpack('!b', b)[0])

    def sync_level(self):
        #print('<- sync_level')
        self.socket.send(struct.pack('!b', Sync.sync_level))
        self.socket.send(struct.pack('!ii', self.level, self.goal))

    def sync_status(self):
        #print('<- sync_status')
        self.socket.send(struct.pack('!b', Sync.sync_status))
        self.socket.send(struct.pack('!iii?iii', 
            self.cur,
            self.player.life, self.player.life_restore, self.player.left,
            self.player.y, self.player.x, self.player.command,
        ))

    def sync_map(self):
        buf = []
        for y, row in enumerate(self.g):
            for x, col in enumerate(row):
                if self.synced_g is None or self.synced_g[y][x]!=col:
                    buf.append(struct.pack('!bbb', y, x, col))
        
        if buf:
            #print('<- sync_map', len(buf))
            self.socket.send(struct.pack('!b', Sync.sync_map))
            self.socket.send(b''.join(buf))

        self.synced_g = copy.deepcopy(self.g)

    def sync_init_level(self):
        #print('<- init_level', self.level)
        self.socket.send(struct.pack('!b', Sync.init_level))

    def sync_show_hud(self, text):
        logger.write(self.uid, ['show_hud', text])
        #print('<- show_hud', text)
        self.socket.send(struct.pack('!b', Sync.show_hud))
        self.socket.send(text.encode('utf-8'))

    def sync_tick_routine(self):
        #print('<- tick_routine')
        self.socket.send(struct.pack('!b', Sync.tick_routine))

    def start_controller(self):
        self.sync_show_hud('欢迎来到神秘小游戏\n完成第 15 级\n可以获得 Flag 1')
        self.sync_show_hud('完成第 15 级\n且总用时不超过 %ss\n可以获得 Flag 2'%GOOD_TIME_S)
        self.init_level(INIT_LEVEL)
        start_time = time.time()
        
        while True:
            try:
                self.player.command = self.wait_command()
                #print('RECV tick', self.player.command)
                self.tick()
            except GameOver:
                self.sync_show_hud('您输了\n最高等级 %d'%self.level)
                self.init_level(INIT_LEVEL)
                start_time = time.time()
            except YouWin:
                time_used = int(time.time()-start_time)
                self.sync_show_hud('您赢了\n进入下一级\n总用时 %ds'%time_used)
                if self.level==15:
                    self.sync_show_hud('Flag 1:\n%s\n你记住了吗？'%getflag(self.token, 0))
                    if time_used<=GOOD_TIME_S:
                        self.sync_show_hud('Flag 2:\n%s\n你记住了吗？'%getflag(self.token, 1))
                self.init_level(self.level+1)

    def init_level(self,level):
        # init grid
        self.g = [[Elem.dirt if random.random()>EVIL_PROB else Elem.evildirt for y in range(GX)] for x in range(GY)]
        self.g[0][math.floor(GX/2)] = Elem.player
        self.g[0][math.floor(GX/2)-1] = Elem.empty
        self.g[0][math.floor(GX/2)+1] = Elem.empty
        
        # put goods
        available = [(y,x) for y in range(GY) for x in range(GX) if self.g[y][x]==Elem.dirt]
        for y, x in random.sample(available, math.ceil(GX*GY*ELEM_PROB)):
            self.g[y][x] = random.choice(Elem.chunks+[Elem.heart])

        # no goods below initial platform
        if self.g[1][math.floor(GX/2)-1]!=Elem.evildirt:
            self.g[1][math.floor(GX/2)-1] = Elem.dirt
        if self.g[1][math.floor(GX/2)]!=Elem.evildirt:
            self.g[1][math.floor(GX/2)] = Elem.dirt
        if self.g[1][math.floor(GX/2)+1]!=Elem.evildirt:
            self.g[1][math.floor(GX/2)+1] = Elem.dirt
        
        # init data
        self.level = level
        self.cur = 0
        self.goal = GOAL_OF_LEVEL(level)
        self.fires = set()
        self.player = Player(self, 0, math.floor(GX/2))

        # trigger client
        self.sync_level()
        self.sync_status()
        self.sync_map()
        self.sync_init_level()
        self.sync_show_hud('等级 %d：\n目标 $%d'%(self.level, self.goal))

    def tick_fire(self):
        for fire in sorted(self.fires, key=lambda this: this.y*GX+this.x):
            try:
                fire.tick()
            except _FireStopped:
                self.g[fire.y][fire.x] = Elem.empty
                self.fires.remove(fire)

    def tick_item(self):
        for y in range(GY-2, -1, -1):
            for x in range(GX):
                if self.g[y][x] in Elem.chunks+[Elem.heart]:
                    if self.g[y+1][x]==Elem.empty:
                        self.g[y+1][x] = self.g[y][x]
                        self.g[y][x] = Elem.empty
                    elif self.g[y+1][x]==Elem.player:
                        if self.g[y][x]==Elem.heart:
                            self.player.life = min(self.player.life+HEART_LIFE, 100)
                        else:
                            self.cur += SALARY[self.g[y][x]]
                            if self.cur>=self.goal:
                                raise YouWin()
                        self.g[y][x] = Elem.empty
                    elif self.g[y+1][x]==Elem.fire:
                        self.new_fire(y,x)

    def new_fire(self,y,x):
        self.g[y][x] = Elem.fire
        self.fires.add(Fire(self, y, x))

    def tick(self):
        self.tick_item()
        if self.fire_ticked:
            self.tick_fire()
        self.fire_ticked = not self.fire_ticked
        self.player.tick()

        self.sync_status()
        self.sync_map()
        self.sync_tick_routine()