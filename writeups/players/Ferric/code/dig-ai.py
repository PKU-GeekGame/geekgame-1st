import struct
from Crypto.PublicKey import RSA
import platform
import securesocket
PSK = b'\x9a6\xfa\x8e\xcf\x82\x15\xd5\x0c\xc5\xa7\xa1\xb9\xd0 28,zO\x1f3!\xa1\xb0K\x9a\xe8\xea*\x15\xc1'
SERVER_ADDR = ('prob10.geekgame.pku.edu.cn', 10010)
SERVER_PUBKEY = RSA.construct((135906273688931402589220204382871553870909804965399698890068262170520688884372045298141155906503818444426612681815829913189804909686158514999348437913468766184178386093808168943112695723474687346392228439951048402868123712946874173233503831688658127511363374069037033641735788196504560132107647546163113244313,
                               65537))
GX = 25
GY = 39


cur_level = 1


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
        self.extension = {}
        self.register(Sync.sync_level, self.on_sync_level)
        self.register(Sync.sync_status, self.on_sync_status)
        self.register(Sync.sync_map, self.on_sync_map)

    def trigger(self, event, *args, **kwargs):
        for callback in self.callbacks.get(event, []):
            callback(*args, **kwargs)

    def register(self, event, callback):
        self.callbacks.setdefault(event, []).append(callback)

    def read_once(self):
        global cur_level

        type = Sync._check(struct.unpack('!b', self.socket.recv(1))[0])
        callbacks = self.callbacks.get(type, [])

        def call(args):
            for callback in callbacks:
                callback(*args)

        if type == Sync.sync_level:
            args = struct.unpack('!ii', self.socket.recv(1024))
            print('sync_level', args)
            call(args)
            return type, args
        elif type == Sync.sync_status:
            args = struct.unpack('!iii?iii', self.socket.recv(1024))
            print('sync_status', args)
            call(args)
            return type, args
        elif type == Sync.sync_map:
            args_all = list(struct.iter_unpack(
                '!bbb', self.socket.recv(None)))
            print('sync_map')
            for args in args_all:
                call(args)
            return type, args_all
        elif type == Sync.init_level:
            print('init_level')
            return type, []
        elif type == Sync.show_hud:
            text = self.socket.recv(None).decode('utf-8')
            print('show_hud', text)
            if '您输了' in text:
                self.display()
                print('cur_level: ', cur_level)
                exit(0)
            if 'flag' in text:
                with open('flag.txt', mode='a+', encoding='utf-8') as f:
                    f.write(text)
            return type, [text]
        elif type == Sync.tick_routine:
            print('tick_routine')
            return type, []
        else:
            print('unknown sync type', type)

    def start(self, token):
        global cur_level
        self.token = token
        self.socket = securesocket.connect(SERVER_ADDR, PSK, SERVER_PUBKEY)
        print('connect')
        self.socket.send(token.encode('utf-8'))
        print('send token')
        self.socket.send(get_platform_name())
        print('send platform', get_platform_name())
        self.read_once()  # Welcome
        self.read_once()  # flag2
        for _ in range(16):
            while True:
                a, b = self.read_once()
                if a == Sync.show_hud and '目标' in b[0]:
                    break

            self.display()

            self.extension = {}
            money, heart, path = self.solve(self.player.y, self.player.x)
            self.walk(path)
            cur_level = cur_level + 1

    def display(self):
        for y in range(GY):
            print(''.join(map(lambda x: material[x] if x != Elem.player else (
                'l' if self.player.left else 'r'), self.g[y])))

    def solve(self, py, px):
        if py == GY - 1:
            return 0, 0, []

        def extend(py, px):
            space = []
            for x in range(px, -1, -1):
                if self.g[py][x] == Elem.evildirt or (py < GY - 1 and self.g[py + 1][x] != Elem.dirt and self.g[py + 1][x] != Elem.evildirt):
                    space.append(x + 1)
                    break
            else:
                space.append(0)
            for x in range(px, GX):
                if self.g[py][x] == Elem.evildirt or (py < GY - 1 and self.g[py + 1][x] != Elem.dirt and self.g[py + 1][x] != Elem.evildirt):
                    space.append(x - 1)
                    break
            else:
                space.append(GX - 1)
            return space

        space = extend(py, px)
        this_money, this_heart = 0, 0
        for x in range(space[0], space[1] + 1):
            if self.g[py][x] == Elem.chunk1:
                this_money = this_money + 1
            elif self.g[py][x] == Elem.chunk2:
                this_money = this_money + 2
            elif self.g[py][x] == Elem.chunk3:
                this_money = this_money + 3
            elif self.g[py][x] == Elem.heart:
                this_heart = this_heart + 1
        extended = [False for _ in range(GX)]
        best_money, best_heart, best_path = -1, -1, []
        for x in range(space[0], space[1] + 1):
            if extended[x]:
                continue
            if self.g[py + 1][x] == Elem.evildirt:
                continue
            deeper = extend(py + 1, x)
            if deeper[0] == deeper[1]:
                continue

            for _x in extended[deeper[0]:deeper[1] + 1]:
                extended[_x] = True
            if (py + 1, (deeper[0], deeper[1])) in self.extension:
                money, heart, path = self.extension[(
                    py + 1, (deeper[0], deeper[1]))]
            else:
                money, heart, path = self.solve(py + 1, x)
                self.extension[(py + 1, (deeper[0], deeper[1]))
                               ] = money, heart, path
            if money + heart > best_money + best_heart:
                best_money, best_heart = money, heart
                best_path = path
        return this_money + best_money, this_heart + best_heart, [space, *best_path]

    def walk(self, path):
        global cur_level
        for i in range(len(path)):
            good_left, good_right = GX, -1
            for x in range(path[i][0], path[i][1] + 1):
                if self.g[self.player.y][x] in Elem.chunks or self.g[self.player.y][x] == Elem.heart:
                    good_left = min(good_left, x)
                    good_right = max(good_right, x)
            # go left
            if not self.player.left:
                self.player.command = Command.left
                if self.tick():
                    return True
            while self.player.x > good_left:
                print('moving to good left: ', good_left)
                if self.g[self.player.y][self.player.x - 1] == Elem.dirt:
                    self.player.command = Command.next
                    if self.tick():
                        return True
                self.player.command = Command.left
                if self.tick():
                    return True
            # go right
            self.player.command = Command.right
            if self.tick():
                return True
            while self.player.x < good_right:
                print('moving to good right: ', good_right)
                if self.g[self.player.y][self.player.x + 1] == Elem.dirt:
                    self.player.command = Command.next
                    if self.tick():
                        return True
                self.player.command = Command.right
                if self.tick():
                    return True

            if i < len(path) - 1:
                deeper = path[i + 1]
                intersection = (max(deeper[0], path[i][0]), min(
                    deeper[1], path[i][1]))
                if self.player.x < intersection[0]:
                    if self.player.left:
                        self.player.command = Command.right
                        if self.tick():
                            return True
                    while self.player.x < intersection[0] - 1:
                        print(f'intersection: {intersection}, moving right')
                        if self.g[self.player.y][self.player.x + 1] == Elem.dirt:
                            self.player.command = Command.next
                            if self.tick():
                                return True
                        self.player.command = Command.right
                        if self.tick():
                            return True
                    self.player.command = Command.down
                    if self.tick():
                        return True
                    self.player.command = Command.right  # go down
                    if self.tick():
                        return True
                    if self.g[self.player.y + 1][self.player.x] != Elem.dirt and self.g[self.player.y + 1][self.player.x] != Elem.evildirt:
                        self.player.command = Command.empty
                        if self.tick():
                            return True
                    if self.g[self.player.y + 1][self.player.x] != Elem.dirt and self.g[self.player.y + 1][self.player.x] != Elem.evildirt:
                        print('Dropdead: ', cur_level)
                        exit(0)
                elif self.player.x > intersection[1]:
                    if not self.player.left:
                        self.player.command = Command.left
                        if self.tick():
                            return True
                    while self.player.x > intersection[1] + 1:
                        print(f'intersection: {intersection}, moving left')
                        if self.g[self.player.y][self.player.x - 1] == Elem.dirt:
                            self.player.command = Command.next
                            if self.tick():
                                return True
                        self.player.command = Command.left
                        if self.tick():
                            return True
                    self.player.command = Command.down
                    if self.tick():
                        return True
                    self.player.command = Command.left  # go down
                    if self.tick():
                        return True
                    if self.g[self.player.y + 1][self.player.x] != Elem.dirt and self.g[self.player.y + 1][self.player.x] != Elem.evildirt:
                        self.player.command = Command.empty
                        if self.tick():
                            return True
                    if self.g[self.player.y + 1][self.player.x] != Elem.dirt and self.g[self.player.y + 1][self.player.x] != Elem.evildirt:
                        print('Dropdead: ', cur_level)
                        exit(0)
                else:
                    if self.player.x == intersection[0]:
                        if self.player.left:
                            self.player.command = Command.right
                            if self.tick():
                                return True
                        self.player.command = Command.down
                        if self.tick():
                            return True
                        self.player.command = Command.right  # go down
                        if self.tick():
                            return True
                        if self.g[self.player.y + 1][self.player.x] != Elem.dirt and self.g[self.player.y + 1][self.player.x] != Elem.evildirt:
                            self.player.command = Command.empty
                            if self.tick():
                                return True
                        if self.g[self.player.y + 1][self.player.x] != Elem.dirt and self.g[self.player.y + 1][self.player.x] != Elem.evildirt:
                            print('Dropdead: ', cur_level)
                            exit(0)
                    else:
                        if not self.player.left:
                            self.player.command = Command.left
                            if self.tick():
                                return True
                        self.player.command = Command.down
                        if self.tick():
                            return True
                        self.player.command = Command.left  # go down
                        if self.tick():
                            return True
                        if self.g[self.player.y + 1][self.player.x] != Elem.dirt and self.g[self.player.y + 1][self.player.x] != Elem.evildirt:
                            self.player.command = Command.empty
                            if self.tick():
                                return True
                        if self.g[self.player.y + 1][self.player.x] != Elem.dirt and self.g[self.player.y + 1][self.player.x] != Elem.evildirt:
                            print('Dropdead: ', cur_level)
                            exit(0)

    def tick(self):
        print('send', command_name[self.player.command])
        self.socket.send(struct.pack('!b', self.player.command))
        sync_map = not((self.player.left and self.player.command == Command.right) or (
            not self.player.left and self.player.command == Command.left))
        type, args = self.read_once()  # sync_status
        if type == Sync.show_hud and '您赢了' in args[0]:
            return True
        if sync_map:
            type, args = self.read_once()  # sync_map
            if type == Sync.show_hud and '您赢了' in args[0]:
                return True
        type, args = self.read_once()  # tick_routine
        if type == Sync.show_hud and '您赢了' in args[0]:
            return True
        self.display()
        return False

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

material = {Elem.chunk1: '1',
            Elem.chunk2: '2',
            Elem.chunk3: '3',
            Elem.dirt: '-',
            Elem.evildirt: '!',
            Elem.empty: ' ',
            Elem.fire: 'x',
            Elem.heart: 'w'}
reverse_material = {
    '1': Elem.chunk1,
    '2': Elem.chunk2,
    '3': Elem.chunk3,
    '-': Elem.dirt,
    '!': Elem.evildirt,
    ' ': Elem.empty,
    'x': Elem.fire,
    'w': Elem.heart
}
command_name = {
    Command.left: 'left',
    Command.right: 'right',
    Command.next: 'up',
    Command.down: 'down',
    Command.empty: 'empty'
}


def start():
    game.start(token)


token = ''


def main():
    start()


main()
