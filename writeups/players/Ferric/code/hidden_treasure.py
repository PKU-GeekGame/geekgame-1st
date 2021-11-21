import curses
import time
import queue
import threading
import struct
import sys
from Crypto.PublicKey import RSA
import platform

import securesocket
PSK = b'\x9a6\xfa\x8e\xcf\x82\x15\xd5\x0c\xc5\xa7\xa1\xb9\xd0 28,zO\x1f3!\xa1\xb0K\x9a\xe8\xea*\x15\xc1'
SERVER_ADDR = ('prob10.geekgame.pku.edu.cn', 10010)
SERVER_PUBKEY = RSA.construct((135906273688931402589220204382871553870909804965399698890068262170520688884372045298141155906503818444426612681815829913189804909686158514999348437913468766184178386093808168943112695723474687346392228439951048402868123712946874173233503831688658127511363374069037033641735788196504560132107647546163113244313,
                               65537))
GX = 25
GY = 39


def get_platform_name():
    u = platform.uname()
    name = '%s (%s %s) on %s' % (u.node, u.system, u.version, u.processor)
    name = name.encode()
    name = name[:500]
    return name


class CheckerMixin:
    @classmethod
    def _check(cls, value):
        for k, v in cls.__dict__.items():
            if not k.startswith('_'):
                if v == value:
                    return v
        else:
            raise RuntimeError('bad value %s for %s' % (value, cls))


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

            if type == Sync.sync_level:
                args = struct.unpack('!ii', self.socket.recv(1024))
                call(args)
            elif type == Sync.sync_status:
                args = struct.unpack('!iii?iii', self.socket.recv(1024))
                call(args)
            elif type == Sync.sync_map:
                args_all = list(struct.iter_unpack(
                    '!bbb', self.socket.recv(None)))
                for args in args_all:
                    call(args)
            elif type == Sync.init_level:
                call([])
            elif type == Sync.show_hud:
                text = self.socket.recv(None).decode('utf-8')
                call([text])
            elif type == Sync.tick_routine:
                call([])
            else:
                print('unknown sync type', type)

        try:
            while True:
                read_once()

        except Exception as e:
            try:
                print(type(e), e)
                exit(1)
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
        self.socket.send(struct.pack('!b', self.player.command))

    def on_sync_level(self, level, goal):
        self.level = level
        self.goal = goal

    def on_sync_status(self, cur, plife, pliferestore, pleft, py, px, pcmd):
        self.cur = cur
        self.player.life = plife
        self.player.life_restore = pliferestore
        self.player.left = pleft
        self.player.y = py
        self.player.x = px
        self.player.command = pcmd

    def on_sync_map(self, y, x, val):
        self.g[y][x] = Elem._check(val)


game = Game()
cmds = queue.Queue()

material = {Elem.chunk1: '1',
            Elem.chunk2: '2',
            Elem.chunk3: '3',
            Elem.dirt: '-',
            Elem.evildirt: '!',
            Elem.empty: ' ',
            Elem.fire: 'x',
            Elem.heart: 'w'}


def cmd(value):
    cmds.put(value)


def show_hud(msg: str):
    global hud_history
    global hud_win
    global hud_history_win

    hud_win.clear()
    # border
    hud_win.addstr(0, 0, '-' * 34 + ' H U D ' + '-' * 34)
    hud_win.addstr(1, 0, msg)
    hud_win.addstr(4, 0, '-' * 75)
    hud_win.refresh()

    hud_history.append(msg.replace('\n', ' || '))
    hud_history_win.clear()
    hud_history_win.addstr(0, 0, '-' * 30 + ' H I S T O R Y ' + '-' * 30)
    for i in range(10):
        hud_history_win.addstr(i + 1, 0, hud_history[-1 - i])
    hud_history_win.addstr(11, 0, '-' * 75)
    hud_history_win.refresh()
    time.sleep(3)


def start():
    def game_controller(token):
        global game

        game.register(Sync.init_level, init_level)
        game.register(Sync.show_hud, show_hud)
        game.register(Sync.tick_routine, tick_routine)
        try:
            game.start(token)
        except Exception as e:
            try:
                print(type(e), repr(e))
                exit(1)
            finally:
                e = None
                del e

        else:
            while True:
                try:
                    if game.player.command == Command.empty:
                        x = cmds.get_nowait()
                        game.player.command = x
                except queue.Empty:
                    game.player.command = Command.empty
                game.tick()
                time.sleep(0.5)

    def game_controller_wrapped(*args):
        try:
            game_controller(*args)
        except Exception as e:
            try:
                print(type(e), e)
                sys.exit(1)
            finally:
                e = None
                del e

    threading.Thread(target=game_controller_wrapped,
                     args=[token], daemon=True).start()


def tick_routine():
    global game_win
    global life_win
    global money_win
    global game

    def get_player_img():
        return 'l' if game.player.left else 'r'

    game_win.clear()
    # border
    game_win.addstr(0, 1, '=' * (GX + 2))
    game_win.addstr(GY + 1, 1, '=' * (GX + 2))
    for y in range(1, GY + 1):
        game_win.addstr(y, 0, '|')
        game_win.addstr(y, GX + 3, '|')
    game_win.addstr(0, 0, '+')
    game_win.addstr(0, GX + 3, '+')
    game_win.addstr(GY + 1, 0, '+')
    game_win.addstr(GY + 1, GX + 3, '+')

    for y in range(GY):
        for x in range(GX):
            if game.g[y][x] == Elem.player:
                game_win.addstr(y + 1, x + 2, get_player_img(),
                                curses.A_STANDOUT)
            else:
                game_win.addstr(y + 1, x + 2, material[game.g[y][x]])
    game_win.refresh()

    life_win.clear()
    life_win.addstr(0, 0, f'life: {game.player.life}')
    life_win.refresh()
    money_win.clear()
    money_win.addstr(0, 0, f'money: ${game.cur}/{game.goal}')
    money_win.refresh()


def clear_cmd_queue():
    try:
        while True:
            cmds.get_nowait()

    except queue.Empty:
        pass
    else:
        game.player.command = Command.empty


def init_level():
    clear_cmd_queue()
    tick_routine()


token = ''
game_win = None
life_win = None
money_win = None
hud_win = None
hud_history_win = None
hud_history = ['' for _ in range(20)]


def main(stdscr):
    global game_win
    global life_win
    global money_win
    global hud_win
    global hud_history_win

    game_win = curses.newwin(45, 31, 1, 1)
    life_win = curses.newwin(1, 20, 1, 32)
    money_win = curses.newwin(1, 20, 2, 32)
    hud_win = curses.newwin(6, 75, 4, 32)
    hud_history_win = curses.newwin(14, 75, 22, 32)

    init_level()
    start()

    while True:
        c = stdscr.getch()
        if c == curses.KEY_UP:
            cmd(Command.next)
        elif c == curses.KEY_DOWN:
            cmd(Command.down)
        elif c == curses.KEY_LEFT:
            cmd(Command.left)
        elif c == curses.KEY_RIGHT:
            cmd(Command.right)


curses.wrapper(main)
