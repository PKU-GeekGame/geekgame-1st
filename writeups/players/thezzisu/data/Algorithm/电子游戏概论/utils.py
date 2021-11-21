GX = 25
GY = 39

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
