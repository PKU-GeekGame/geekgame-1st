import threading
import struct
import sys
import hashlib
from Crypto.PublicKey import RSA

import securesocket

SERVER_ADDR = ('prob10.geekgame.pku.edu.cn', 10010)
#SERVER_ADDR = ('127.0.0.1', 12222)
SERVER_PUBKEY = RSA.construct((0xc1897613762f133869db8ada9eeddffb9b3b87d2a275cb7eebd1e6739e3b4424bc1a6fa513ddcc5ed43d2a9dfaff9b318f9de6e34c715ac5ff975cfb81c4d7d25974adda7dc30bf8854a888b1feb18c8b3a4c0c924e4ed538e52f214dea1ac863c7553d3d78a443be0dacaee1d777dc8668d608959cd46ff5f9d6e35a5543699, 65537))
GX = 25
GY = 39

with open('textures.msi', 'rb') as f:
    PSK = hashlib.sha256(f.read()).digest()
    assert len(PSK)==32

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

class Player:
    def __init__(self, game, y, x):
        self.life = 100
        self.life_restore = 0
        #self.life_waiting = False
        self.left = False
        self.command = Command.empty
        #self.game = game
        self.y = y
        self.x = x


class Game:
    def __init__(self):
        self.g = [[Elem.dirt for y in range(GX)] for x in range(GY)]
        self.level = -1
        self.cur = 0
        self.goal = 0
        #self.fires = set()
        self.player = Player(self, 0, 0)
        #self.fire_ticked = False
        
        self.callbacks = {}
        self.socket = None
        self.token = None

        self.register(Sync.sync_level, self.on_sync_level)
        self.register(Sync.sync_status, self.on_sync_status)
        self.register(Sync.sync_map, self.on_sync_map)

    def trigger(self, event, *args, **kwargs):
        for callback in self.callbacks.get(event, []):
            callback(*args, **kwargs)

    def register(self, event, callback):
        self.callbacks.setdefault(event, []).append(callback)

    def socket_listener(self):
        def read_once():
            type = Sync._check(struct.unpack('!b', self.socket.recv(1))[0])
            callbacks = self.callbacks.get(type, [])
            if not callbacks:
                return

            def call(args):
                for callback in callbacks:
                    callback(*args)

            if type==Sync.sync_level:
                args = struct.unpack('!ii', self.socket.recv(1024))
                call(args)
            elif type==Sync.sync_status:
                args = struct.unpack('!iii?iii', self.socket.recv(1024))
                call(args)
            elif type==Sync.sync_map:
                args_all = list(struct.iter_unpack('!bbb', self.socket.recv(None)))
                for args in args_all:
                    call(args)
            elif type==Sync.init_level:
                call([])
            elif type==Sync.show_hud:
                text = self.socket.recv(None).decode('utf-8')
                call([text])
            elif type==Sync.tick_routine:
                call([])
            else:
                print('unknown sync type', type)

        try:
            while True:
                read_once()
        except Exception as e:
            print(type(e), repr(e))
            sys.exit(1)

    def start(self, token):
        self.token = token
        self.socket = securesocket.connect(SERVER_ADDR, PSK, SERVER_PUBKEY)
        self.socket.send(token.encode('utf-8'))
        self.socket.send(b'emulator')
        threading.Thread(target=self.socket_listener, daemon=True).start()

    def tick(self):
        #print('-> tick', self.player.command)
        self.socket.send(struct.pack('!b', self.player.command))

    def on_sync_level(self, level, goal):
        #print('GOT sync_level', level, goal)
        self.level = level
        self.goal = goal

    def on_sync_status(self, cur, plife, pliferestore, pleft, py, px, pcmd):
        #print('GOT sync_status')
        self.cur = cur
        self.player.life = plife
        self.player.life_restore = pliferestore
        self.player.left = pleft
        self.player.y = py
        self.player.x = px
        self.player.command = pcmd

    def on_sync_map(self, y, x, val):
        #print('GOT sync_map', y, x)
        self.g[y][x] = Elem._check(val)