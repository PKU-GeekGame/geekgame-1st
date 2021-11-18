import libremote as lib
from tkinter import *
from tkinter.ttk import *
from tkinter import simpledialog
import copy

TOKEN = '...'

tk = Tk()
hpmsg = StringVar(value=' HP')
moneymsg = StringVar(value='Money ')

f = Frame(tk)
f.grid(row=0, column=0, sticky='we')
f.columnconfigure(2, weight=1)

lifebar = Progressbar(f, orient=HORIZONTAL, length=80, value=100, maximum=100, mode='determinate')
lifebar.grid(row=0, column=0)
Label(f, textvariable=hpmsg).grid(row=0, column=1)
Label(f).grid(row=0, column=2, sticky='we')
Label(f, textvariable=moneymsg).grid(row=0, column=3)
moneybar = Progressbar(f, orient=HORIZONTAL, length=80, value=0, maximum=1, mode='determinate')
moneybar.grid(row=0, column=4)


t = Text(tk, font='monospace -15', height=lib.GY+1, width=lib.GX*2)
t.grid(row=1, column=0)

game = lib.Game()
token = TOKEN

def describe():
    return '\n'.join([''.join([pretty_table[x] for x in row]) for row in game.g])

def redraw(*_):
    print('redraw')
    t.delete(1.0, END)
    t.insert(END, describe()+'\n')
    tk.title('Lv %d'%game.level)
    hpmsg.set(f' {game.player.life} +{game.player.life_restore}')
    moneymsg.set(f'Need {game.goal-game.cur} ')
    lifebar['value'] = game.player.life
    moneybar['value'] = game.cur
    automove()

def cmd(value):
    game.player.command = value
    game.tick()

def automove():
    print(game.player.y, game.player.x)
    if game.g[game.player.y+1][game.player.x] in lib.Elem.chunks+[lib.Elem.heart, lib.Elem.empty]:
        print('automove GET DOWN')
        cmd(lib.Command.empty)
    elif game.player.y>0 and game.g[game.player.y-1][game.player.x] in lib.Elem.chunks+[lib.Elem.heart]:
        print('automove GET UP')
        cmd(lib.Command.empty)

pretty_table = {
    lib.Elem.chunk1: '①',
    lib.Elem.chunk2: '②',
    lib.Elem.chunk3: '③',
    lib.Elem.dirt: '■',
    lib.Elem.evildirt: '▒',
    lib.Elem.empty: '　',
    lib.Elem.heart: '☆',
    lib.Elem.fire: '卐',
    lib.Elem.player: 'Ｐ',
}

def go_left():
    if game.player.x==0:
        return

    if not game.player.left:
        cmd(lib.Command.left)
    if game.g[game.player.y][game.player.x-1] in [lib.Elem.dirt, lib.Elem.evildirt]:
        cmd(lib.Command.next)
    cmd(lib.Command.left)

def go_right():
    if game.player.x==lib.GX-1:
        return

    if game.player.left:
        cmd(lib.Command.right)
    if game.g[game.player.y][game.player.x+1] in [lib.Elem.dirt, lib.Elem.evildirt]:
        cmd(lib.Command.next)
    cmd(lib.Command.right)

def digg_left():
    if game.player.x==0:
        return

    if not game.player.left:
        cmd(lib.Command.left)
    cmd(lib.Command.down)
    cmd(lib.Command.left)

def digg_right():
    if game.player.x==lib.GX-1:
        return

    if game.player.left:
        cmd(lib.Command.right)
    cmd(lib.Command.down)
    cmd(lib.Command.right)

def dash_left():
    if not game.player.left:
        cmd(lib.Command.left)
    g = copy.deepcopy(game.g)
    y = game.player.y
    x = game.player.x
    while x>0 and g[y][x-1] not in lib.Elem.chunks+[lib.Elem.heart]:
        if g[y][x-1]==lib.Elem.evildirt:
            return
        if g[y][x-1]==lib.Elem.dirt:
            cmd(lib.Command.next)
        cmd(lib.Command.left)
        x -= 1
        if y>0 and g[y-1][x] in lib.Elem.chunks+[lib.Elem.heart]: # fall up
            return
        if y<lib.GY-1 and g[y+1][x] not in [lib.Elem.dirt, lib.Elem.evildirt]: # fall
            return
    cmd(lib.Command.left)

def dash_right():
    if game.player.left:
        cmd(lib.Command.right)
    g = copy.deepcopy(game.g)
    y = game.player.y
    x = game.player.x
    while x<lib.GX-1 and g[y][x+1] not in lib.Elem.chunks+[lib.Elem.heart]:
        if g[y][x+1]==lib.Elem.evildirt:
            return
        if g[y][x+1]==lib.Elem.dirt:
            cmd(lib.Command.next)
        cmd(lib.Command.right)
        x += 1
        if y>0 and g[y-1][x] in lib.Elem.chunks+[lib.Elem.heart]: # fall up
            return
        if y<lib.GY-1 and g[y+1][x] not in [lib.Elem.dirt, lib.Elem.evildirt]: # fall down
            return
    cmd(lib.Command.right)


tk.bind('<Left>', lambda *_: dash_left())
tk.bind('<Right>', lambda *_: dash_right())
tk.bind('<Control-Left>', lambda *_: digg_left())
tk.bind('<Control-Right>', lambda *_: digg_right())
tk.bind('<Shift-Left>', lambda *_: go_left())
tk.bind('<Shift-Right>', lambda *_: go_right())

def show_hud(text):
    #messagebox.showinfo('HUD', text)
    print('HUD', text)

def init_level():
    moneybar['maximum'] = game.goal
    redraw()

game.register(lib.Sync.tick_routine, redraw)
game.register(lib.Sync.init_level, init_level)
game.register(lib.Sync.show_hud, show_hud)
game.start(token)
redraw()
mainloop()