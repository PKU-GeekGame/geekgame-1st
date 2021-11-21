# uncompyle6 version 3.8.0
# Python bytecode 3.8.0 (3413)
# Decompiled from: Python 3.10.0 (tags/v3.10.0:b494f59, Oct  4 2021, 19:00:18) [MSC v.1929 64 bit (AMD64)]
# Embedded file name: hidden_treasure.pyw
import time, queue, msilib, threading, base64, struct, sys, hashlib
from Crypto.PublicKey import RSA
import platform, securesocket
from utils import *
import solver
with open('textures.msi', 'rb') as (f):
    PSK = hashlib.sha256(f.read()).digest()
    assert len(PSK) == 32
SERVER_ADDR = ('prob10.geekgame.pku.edu.cn', 10010)
SERVER_PUBKEY = RSA.construct((135906273688931402589220204382871553870909804965399698890068262170520688884372045298141155906503818444426612681815829913189804909686158514999348437913468766184178386093808168943112695723474687346392228439951048402868123712946874173233503831688658127511363374069037033641735788196504560132107647546163113244313,
                               65537))

TOKEN = 'TOKEN'

def get_platform_name():
    u = platform.uname()
    name = '%s (%s %s) on %s' % (u.node, u.system, u.version, u.processor)
    name = name.encode()
    name = name[:500]
    print(name)
    return name

class Player:

    def __init__(self, game, y, x):
        self.life = 100
        self.life_restore = 0
        self.left = False
        self.command = Command.empty
        self.y = y
        self.x = x


class Game:

    def __init__(self):
        self.g = [[Elem.dirt for y in range(GX)] for x in range(GY)]
        self.level = -1
        self.cur = 0
        self.goal = 0
        self.player = Player(self, 0, 0)
        self.callbacks = {}
        self.socket = None
        self.token = None
        self.register(Sync.sync_level, self.on_sync_level)
        self.register(Sync.sync_status, self.on_sync_status)
        self.register(Sync.sync_map, self.on_sync_map)
        self.lastcnt = 0

    def trigger(self, event, *args, **kwargs):
        for callback in self.callbacks.get(event, []):
            callback(*args, **kwargs)

    def register(self, event, callback):
        self.callbacks.setdefault(event, []).append(callback)

    def socket_listener(self):

        def read_once():
            type = Sync._check(struct.unpack('!b', self.socket.recv(1))[0])
            # print('Sync', type)
            callbacks = self.callbacks.get(type, [])
            if not callbacks:
                return

            def call(args):
                for callback in callbacks:
                    callback(*args)

            if type == Sync.sync_level:
                args = struct.unpack('!ii', self.socket.recv(1024))
                call(args)
            elif type == Sync.sync_status:
                args = struct.unpack('!iii?iii', self.socket.recv(1024))
                call(args)
            else:
                if type == Sync.sync_map:
                    args_all = list(struct.iter_unpack('!bbb', self.socket.recv(None)))
                    for args in args_all:
                        call(args)

                else:
                    if type == Sync.init_level:
                        call([])
                    else:
                        if type == Sync.show_hud:
                            text = self.socket.recv(None).decode('utf-8')
                            call([text])
                        else:
                            if type == Sync.tick_routine:
                                call([])
                            else:
                                print('unknown sync type', type)

        try:
            while True:
                read_once()

        except Exception as e:
            try:
                print(type(e), repr(e))
                print('神秘小游戏', '与服务器的连接中断！')
                sys.exit(1)
            finally:
                e = None
                del e

    def start(self, token):
        self.token = token
        self.socket = securesocket.connect(SERVER_ADDR, PSK, SERVER_PUBKEY)
        self.socket.send(token.encode('utf-8'))
        self.socket.send(get_platform_name())
        threading.Thread(target=(self.socket_listener), daemon=True).start()

    def tick(self):
        if self.player.command != Command.empty:
            # print("Game cmd:",self.player.command, flush=True)
            self.socket.send(struct.pack('!b', self.player.command))
            # self.player.command = Command.empty
    def on_sync_level(self, level, goal):
        self.level = level
        self.goal = goal
        clear_cmd_queue()

    def on_sync_status(self, cur, plife, pliferestore, pleft, py, px, pcmd):
        if (self.player.command == Command.left or self.player.command == Command.right) and pcmd == Command.empty:
            assert self.player.x != px or self.player.y != py or self.player.left != pleft
        self.cur = cur
        self.player.life = plife
        self.player.life_restore = pliferestore
        self.player.left = pleft
        self.player.y = py
        self.player.x = px
        # print('Player', px, py, pleft, pcmd, flush=True)
        if pcmd == Command.empty:
            self.player.command = pcmd
        else:
            # print('Resend', flush=True)
            self.socket.send(struct.pack('!b', pcmd))

    def on_sync_map(self, y, x, val):
        self.g[y][x] = Elem._check(val)

game = Game()
cmds = queue.Queue()
# TICKTIME = 0.001

def show_hud(msg):
    print(msg, file=sys.stderr)


def start():

    def game_controller(token):
        game.register(Sync.init_level, init_level)
        game.register(Sync.show_hud, show_hud)
        try:
            game.start(token)
        except Exception as e:
            try:
                print(type(e), repr(e))
                print('神秘小游戏', '服务器连接失败！')
                sys.exit(1)
            finally:
                e = None
                del e

        else:
            while True:
                try:
                    if game.player.command == Command.empty:
                        x = cmds.get_nowait()
                        game.player.command = x
                        game.tick()
                except queue.Empty:
                    pass

    def game_controller_wrapped():
        try:
            game_controller(TOKEN)
        except Exception as e:
            try:
                print(type(e), e)
                sys.exit(1)
            finally:
                e = None
                del e

    game_controller_wrapped()

def clear_cmd_queue():
    try:
        while True:
            cmds.get_nowait()
    except queue.Empty:
        pass
    game.player.command = Command.empty

def init_level():
    clear_cmd_queue()
    commands = solver.better_solve(game.g)
    for command in commands:
        cmds.put(command)

start()