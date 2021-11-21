# uncompyle6 version 3.8.0
# Python bytecode 3.8.0 (3413)
# Decompiled from: Python 3.10.0 (tags/v3.10.0:b494f59, Oct  4 2021, 19:00:18) [MSC v.1929 64 bit (AMD64)]
# Embedded file name: hidden_treasure.pyw
from tkinter import *
from tkinter import messagebox
from tkinter import simpledialog
from tkinter.ttk import *
import time, queue, msilib, threading, base64, struct, sys, hashlib
from utils import *

tk = Tk()
tk.title('神秘小游戏')
tk.resizable(False, False)
DESCRIPTION = '按方向键移动。玩法不太好讲清楚，多试几次应该就明白了。另外急招美工一名，还有测试工程师。'
SZ = 32
BORDER = 10
TICKTIME = 0.05
paused = False
moneymsg = StringVar()
hudvar = StringVar()
dg = [[None for x in range(GX)] for y in range(GY)]
onscreen = [[None for x in range(GX)] for y in range(GY)]
material = {}
player = {}

def load_texture():
    global material
    global player
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

    material = {Elem.chunk1: load_image('chunk1'), 
     Elem.chunk2: load_image('chunk2'), 
     Elem.chunk3: load_image('chunk3'), 
     Elem.dirt: load_image('dirt'), 
     Elem.evildirt: load_image('playerHurtL'), 
     Elem.empty: load_image('empty'), 
     Elem.fire: load_image('fire'), 
     Elem.heart: load_image('heart')}
    player = {'LeftNormal':load_image('playerL'), 
     'RightNormal':load_image('playerR'), 
     'LeftHurt':load_image('playerHurtL'), 
     'RightHurt':load_image('playerHurtR')}
    db.Close()

load_texture()

def draw(g):
    # print("tick_routine")
    global dg
    global onscreen

    def get_player_img():
        return player[(('Left' if True else 'Right') + ('Hurt' if False else 'Normal'))]

    for y in range(GY):
        for x in range(GX):
            # print(y, x)
            if g[y][x] != onscreen[y][x] or g[y][x] == Elem.player:
                onscreen[y][x] = g[y][x]
                if dg[y][x]:
                    canvas.delete(dg[y][x])
                if g[y][x] == Elem.player:
                    dg[y][x] = canvas.create_image((x * SZ), (y * SZ), anchor='nw', image=(get_player_img()))
                else:
                    dg[y][x] = canvas.create_image((x * SZ), (y * SZ), anchor='nw', image=(material[g[y][x]]))

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
canvas = Canvas(tk, width=(GX * SZ), height=(GY * SZ), bg='#770055')
canvas.grid(row=1, column=0, sticky='nswe')
hud = Label(tk, textvariable=hudvar, background='#000055', foreground='#ffffff', font='黑体 -25')
infof = Frame(tk)
infof.grid(row=2, column=0, sticky='we')
infof.columnconfigure(1, weight=1)
pausebtn = Button(infof, text='暂停', width=8, command=lambda: 0)
pausebtn.grid(row=0, column=0)
Label(infof).grid(row=0, column=1, sticky='we')
Button(infof, text='操作说明', width=8, command=(lambda : messagebox.showinfo('操作说明', DESCRIPTION))).grid(row=0, column=2, columnspan=2)

def start():
  mainloop()
