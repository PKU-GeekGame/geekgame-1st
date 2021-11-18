#coding=utf-8
from tkinter import *
from tkinter import messagebox
from tkinter import simpledialog
from tkinter.ttk import *

import time
import queue
import msilib
import threading
import base64
import struct
import sys
import hashlib
from Crypto.PublicKey import RSA
import platform

import securesocket

with open('textures.msi', 'rb') as f:
    PSK = hashlib.sha256(f.read()).digest()
    assert len(PSK)==32

#SERVER_ADDR = ('127.0.0.1', 10010)
SERVER_ADDR = ('prob10.geekgame.pku.edu.cn', 10010)
SERVER_PUBKEY = RSA.construct((0xc1897613762f133869db8ada9eeddffb9b3b87d2a275cb7eebd1e6739e3b4424bc1a6fa513ddcc5ed43d2a9dfaff9b318f9de6e34c715ac5ff975cfb81c4d7d25974adda7dc30bf8854a888b1feb18c8b3a4c0c924e4ed538e52f214dea1ac863c7553d3d78a443be0dacaee1d777dc8668d608959cd46ff5f9d6e35a5543699, 65537))
GX = 25
GY = 39

def get_platform_name():
    u = platform.uname()
    name = '%s (%s %s) on %s'%(u.node, u.system, u.version, u.processor)
    name = name.encode()
    name = name[:500]
    return name

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
            messagebox.showerror('神秘小游戏', '与服务器的连接中断！')
            tk.destroy()
            sys.exit(1)

    def start(self, token):
        self.token = token
        self.socket = securesocket.connect(SERVER_ADDR, PSK, SERVER_PUBKEY)
        self.socket.send(token.encode('utf-8'))
        self.socket.send(get_platform_name())
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

tk = Tk()
tk.title('神秘小游戏')
tk.resizable(False, False)

DESCRIPTION = '按方向键移动。玩法不太好讲清楚，多试几次应该就明白了。另外急招美工一名，还有测试工程师。'
game = Game()
cmds = queue.Queue()
SZ = 32
BORDER = 10
#TICKTIME = .1
TICKTIME = .4
paused = False
moneymsg = StringVar()
hudvar = StringVar()

dg = [[None for x in range(GX)] for y in range(GY)]
onscreen = [[None for x in range(GX)] for y in range(GY)]

material = {}
player = {}

def load_texture():
    db = msilib.OpenDatabase('textures.msi', msilib.MSIDBOPEN_READONLY)
    def get_key(db, k):
        v = db.OpenView('select `Text` from UIText where `Key`=?')
        args = msilib.CreateRecord(1)
        args.SetString(1, k)
        v.Execute(args)
        rec = v.Fetch()
        ret = rec.GetString(1)
        v.Close()
        return ret

    def load_image(key):
        data = get_key(db, key)
        data = base64.b64decode(data.encode())
        return PhotoImage(data=data)

    global material
    global player

    material = {
        Elem.chunk1: load_image('chunk1'),
        Elem.chunk2: load_image('chunk2'),
        Elem.chunk3: load_image('chunk3'),
        Elem.dirt: load_image('dirt'),
        Elem.evildirt: load_image('evildirt'),
        Elem.empty: load_image('empty'),
        Elem.fire: load_image('fire'),
        Elem.heart: load_image('heart'),
    }
    player = {
        'LeftNormal': load_image('playerL'),
        'RightNormal': load_image('playerR'),
        'LeftHurt': load_image('playerHurtL'),
        'RightHurt': load_image('playerHurtR'),
    }
    db.Close()

def pause(force=None):
    global paused
    if force is None:
        paused = not paused
    else:
        paused = force
    pausebtn.state(['pressed' if paused else '!pressed'])
    pausebtn['text'] = '已暂停' if paused else '暂停'

def cmd(value):
    cmds.put(value)

def show_hud(msg):
    hudvar.set(msg)
    hud.grid(row=1, column=0)
    
    pause(True)
    pausebtn.state(['disabled'])
    time.sleep(3)
    pausebtn.state(['!disabled'])
    pause(False)

    hud.grid_forget()

def start():
    def game_controller(token):
        load_texture()
        game.register(Sync.init_level, init_level)
        game.register(Sync.show_hud, show_hud)
        game.register(Sync.tick_routine, tick_routine)
        
        try:
            game.start(token)
        except Exception as e:
            print(type(e), repr(e))
            messagebox.showerror('神秘小游戏', '服务器连接失败！')
            tk.destroy()
            sys.exit(1)

        while True:
            if paused:
                time.sleep(.1)
                continue

            try:
                if game.player.command==Command.empty:
                    x = cmds.get_nowait()
                    game.player.command = x
            except queue.Empty:
                game.player.command = Command.empty
            
            game.tick()
            time.sleep(TICKTIME)

    def game_controller_wrapped(*args):
        try:
            game_controller(*args)
        except Exception as e:
            print(type(e), e)
            sys.exit(1)

    token = simpledialog.askstring('神秘小游戏', 'Token:', parent=tk)
    if not token:
        tk.destroy()
        sys.exit(1)
    else:
        threading.Thread(target=game_controller_wrapped, args=[token], daemon=True).start()

def tick_routine(redraw=False):
    def get_player_img():
        return player[('Left' if game.player.left else 'Right')+('Hurt' if game.player.life_restore else 'Normal')]
    #update canvas
    for y in range(GY):
        for x in range(GX):
            if redraw or game.g[y][x] is not onscreen[y][x] or game.g[y][x]==Elem.player:
                onscreen[y][x] = game.g[y][x]
                if dg[y][x]:
                    canvas.delete(dg[y][x])
                if game.g[y][x]==Elem.player:
                    dg[y][x] = canvas.create_image(x*SZ, y*SZ, anchor='nw', image=get_player_img())
                else:
                    dg[y][x] = canvas.create_image(x*SZ, y*SZ, anchor='nw', image=material[game.g[y][x]])
    #update data
    moneymsg.set('$%d/%d '%(game.cur, game.goal))
    moneybar['value'] = game.cur
    lifebar['value'] = game.player.life
    #focus on player
    canvas.yview_moveto((BORDER+game.player.y-4)/(GY+2*BORDER))
    canvas.xview_moveto((BORDER+game.player.x-4)/(GX+2*BORDER))

def clear_cmd_queue():
    try:
        while True: # clear queue
            cmds.get_nowait()
    except queue.Empty:
        pass
    game.player.command = Command.empty

def init_level():
    global dg
    global onscreen
    dg = [[None for x in range(GX)] for y in range(GY)]
    onscreen = [[None for x in range(GX)] for y in range(GY)]
    
    canvas.delete('all')
    canvas['scrollregion'] = (-BORDER*SZ, -BORDER*SZ, (BORDER+GX)*SZ, (BORDER+GY)*SZ)
    
    moneybar['value'] = 0
    moneybar['maximum'] = game.goal

    clear_cmd_queue()
    tick_routine(redraw=True)

f = Frame(tk)
f.grid(row=0, column=0, sticky='we')
f.columnconfigure(2, weight=1)

lifebar = Progressbar(f, orient=HORIZONTAL, length=80, value=100, maximum=100, mode='determinate')
lifebar.grid(row=0, column=0)
Label(f, text=' HP').grid(row=0, column=1)
Label(f).grid(row=0, column=2, sticky='we')
Label(f, textvariable=moneymsg).grid(row=0, column=3)
moneybar = Progressbar(f, orient=HORIZONTAL, length=80, value=0, maximum=1, mode='determinate')
moneybar.grid(row=0, column=4)

canvas = Canvas(tk, width=9*SZ, height=9*SZ, bg='#770055')
canvas.grid(row=1, column=0, sticky='nswe')
hud = Label(tk, textvariable=hudvar, background='#000055', foreground='#ffffff', font='黑体 -25')

infof = Frame(tk)
infof.grid(row=2, column=0, sticky='we')
infof.columnconfigure(1, weight=1)

pausebtn = Button(infof, text='暂停', width=8, command=pause)
pausebtn.grid(row=0, column=0)
Label(infof).grid(row=0, column=1, sticky='we')
Button(infof, text='操作说明', width=8, command=lambda: messagebox.showinfo('操作说明', DESCRIPTION)).grid(row=0, column=2, columnspan=2)

tk.bind_all('<Left>', lambda *_: cmd(Command.left))
tk.bind_all('<Right>', lambda *_: cmd(Command.right))
tk.bind_all('<Up>', lambda *_: cmd(Command.next))
tk.bind_all('<Down>', lambda *_: cmd(Command.down))
tk.after(1, lambda *_: start())
mainloop()