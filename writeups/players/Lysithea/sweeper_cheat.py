import random, time
from randcrack import RandCrack
import sweeper
# from asyncio import Queue,open_connection,gather,run,sleep
import socket, threading, queue
import re
from itertools import product

server = ('prob09.geekgame.pku.edu.cn', 10009)
token = '1027:MEYCIQDHZe2CIK6PqeGWv+ddzpl4ruf8X3b8QiuXgKe4rCITNwIhANPHLthaEHKzoiIWqMklsnG3B+FVhaQlwhPyTdJin59H'

class cmd_socket():
    def __init__(self) -> None:
        self.socket = socket.create_connection(server, timeout=0.002)
        self.send_queue = queue.Queue()
        self.recv_queue = queue.Queue()

    def send(self, msg:str):
        self.send_queue.put(msg + '\n')

    def recv(self, timeout=0.002)->str:
        return self.recv_queue.get(timeout=timeout)

    def loop(self, timeout=0.002):
        buf = b''
        EOF_flag = False
        while not EOF_flag:
            try:
                # first recv
                while True:
                    tmp = self.socket.recv(4096)
                    if tmp:
                        buf += tmp
                    else:
                        EOF_flag = True
                        break
            except socket.timeout as e:
                # pack msg and print
                pass
            finally:
                if buf:
                    msg = buf.decode()
                    buf = b''
                    print(msg, end='')
                    self.recv_queue.put(msg)
                if EOF_flag:
                    self.recv_queue.put(None)

            try:
                # then send
                msg = self.send_queue.get(timeout=timeout)
                # \033[0;32;40m我是小杨我就这样\033[0m
                print('\033[0;33;40m',msg,'\033[0m',sep='',end='')
                self.socket.send(msg.encode())
            except queue.Empty as e:
                pass
        self.socket.close()

    def drain_recv(self):
        try:
            yield self.recv()
        except queue.Empty as e:
            return

    
    def close(self):
        self.socket.close()

    def recv_till_pending(self, ptn='^[\S\s]+\n> $', maxtime = 1000)->str:
        '''
        here pending means end with '\n> '
        '''
        msg_read = ''
        cur_time = 0
        while not re.search(ptn, msg_read):
            tmp = tuple(self.drain_recv())
            if None in tmp:
                # EOF
                msg_read += ''.join(tmp[:-1])
                break
            msg_read += ''.join(tmp)
            cur_time += 1
            if cur_time > maxtime:
                raise Exception('pending till maxtime')
        return msg_read

def do_hard(cmd:cmd_socket, pretend=78):
    rc = RandCrack()
    cmd.send('n')
    for _ in range(pretend):
        welcome_str = cmd.recv_till_pending()
        for i, j in product(range(16), range(16)):
            cmd.send('%d %d'%(i,j))
            res = cmd.recv_till_pending('(^[\S\s]+\n> $)|([\S\s]+\(y/n\))|([\S\s]+You win[\S\s]+)')
            if re.search('^[\S\s]+\n> $', res):
                # typical out: '5\n>'
                continue
            elif res[0:5] == 'BOOM!':
                # typical out:
                # BOOM!
                # *****4222*4**4**
                # **65***3*4***6*4
                # ***22434*335**3*
                # 453102*4322*4442
                # **1125**4*313**2
                # 3311****4*313**3
                # *21357*544*45*7*
                # *53*****3*******
                # ***6*6**336**553
                # 36*5*55532**6*5*
                # 1**53****345****
                # 24*3*5**42**35*5
                # 2*4314**322323**
                # 3**202*6*433*233
                # *7*4244*****434*
                # ***3***334**3***

                # try again? (y/n)
                raw_map = ''.join(res.split('\n')[1:17])
                assert len(raw_map) == 16*16
                raw_map = re.sub('[0-9]','0',raw_map)
                raw_map = re.sub('\*','1',raw_map)
                # the first generated int is at end of number
                for bindex in range(8):
                    rc.submit(int(raw_map[(bindex+1)*32-1-256:bindex*32-1-256:-1], 2))

                cmd.send('y')
                break
            elif res[0:7] == 'You win':
                print('\033[0;31;40m', res,'\033[0m',sep='',end='')
            else:
                print('\033[0;31;40m', res,'\033[0m',sep='',end='')
                raise(Exception('Unknown circumstances'))

    xin_ling_shu = rc.predict_getrandbits(16*16)
    real_map = format(xin_ling_shu, 'b').zfill(256)[::-1]
    print('\n======CHEATING MODE=====')
    
    welcome_str = cmd.recv_till_pending()

    for i, j in ((i,j) for i in range(16) for j in range(16) if real_map[i*16+j]=='0'):
        cmd.send('%d %d'%(i,j))
        res = cmd.recv_till_pending('(^[\S\s]+\n> $)|([\S\s]+\(y/n\))|(You win)')
        if re.search('^[\S\s]+\n> $', res):
            # typical out: '5\n>'
            continue
        elif res[0:5] == 'BOOM!':
            print('\033[0;31;40m', real_map,'\033[0m',sep='',end='')
            raise(Exception('Prediction Fail'))
        elif re.search('You win', res):
            print('\033[0;36;40m', res,'\033[0m',sep='',end='')
        else:
            print('\033[0;31;40m', res,'\033[0m',sep='',end='')
            raise(Exception('Unknown circumstances'))
    cmd.close()

def do_easy(cmd:cmd_socket):
    cmd.send('y')

def valid_board(bits:int):
    WIDTH = 16
    HEIGHT = 16
    board = [[None] * WIDTH for _ in range(HEIGHT)]
    for i in range(HEIGHT):
        for j in range(WIDTH):
            x = (bits >> (i * WIDTH + j)) & 1
            board[i][j] = x
    return board

if __name__ == '__main__':
    cmd = cmd_socket()
    threading.Thread(target=cmd.loop).start()
    cmd.send(token)
    do_hard(cmd)
    



# class geek_terminal():
#     def __init__(self) -> None:
#         self.reader, self.writer = None, None
#         self.tasks = []
#         self.pending = True
#         self._read_tick = 0
#         self.buf = b''

#     async def conn(self):
#         self.reader, self.writer = await open_connection(*server)

#     async def recv(self, bytelen:int):
#         tmp = await self.reader.read(bytelen)
#         print(self.tmp.decode(),end='')
#         return self.tmp.decode()

#     async def send(self):
#         while True:
#             msg = await self.write_stream.get()
#             print(msg,end='')
#             self.writer.write(msg.encode())
#             await sleep(0.5)

#     def close(self):
#         for task in self.tasks:
#             task.cancel()
#         self.reader.close()
#         self.writer.close()

#     async def run(self):
#         self.read_stream = Queue(8192)
#         self.write_stream = Queue(100)   
#         self.tasks = [self.conn()]
#         await gather(*self.tasks)
#         print('Connect Suc')
#         self.tasks = [self.recv(), self.send(), self.bot()]
#         await gather(*self.tasks, return_exceptions=False)

#     async def bot(self):
        
#         self.write_stream.put_nowait(token+'\n')
#         _=await self.read_stream.get()
#         _=await self.read_stream.get()
#         for _ in range(10):
#             await self.pretend_play()

#     async def pretend_play(self):
#         self.write_stream.put_nowait('n\n')




