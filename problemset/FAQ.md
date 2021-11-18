# 选手常见问题

## 关于计分

竞赛采用动态分值机制，<b style="color: darkred">第一阶段解出的校内选手数量越多，该题分数越少。</b>

具体来说，如果一道基础分值为 x 的题目在第一阶段结束时（18 日 12:00）有 n 名校内选手解出，则所有在第一阶段解出的选手实际获得 <code>x * (0.4 + 0.6 * 0.98<sup> max(0, n-1)</sup>)</code> 分（向下取整到整数）。

第二阶段解出的题目实际获得的分数是上述计算结果的 1/4，这些题目会标记为棕色，且在图标旁边显示一个星号。



## 关于终端交互

带有终端交互的题目会在服务器上运行一个特定的程序。每次通过终端连接到题目时，会启动该程序，允许你向它输入内容，并读取输出。你可以将程序下载到本地进行分析或调试，找出其中的漏洞，并攻击服务器获得 Flag。

除了使用网页终端以外，你也可以使用命令行工具 netcat (<code>nc</code>) 来连接到服务器，或者使用任意编程语言带 socket 通信功能的库（如 Python 的 pwntools）。

<b style="color: darkred">终端交互的频率限制是 30 秒内最多连接 3 次。</b>

<b style="color: darkred">除 XSS Bot 以外的题目环境均没有外网，</b>因此无法反弹 Shell。

<b style="color: darkred">请注意网页终端和 tty 有每行最多 4096 字符的限制，</b>如果需要输入很多内容建议利用输入重定向或使用 pwntools。参见 <a href="https://lug.ustc.edu.cn/planet/2019/09/how-to-use-nc/#%E6%B3%A8%E6%84%8F%E4%BA%8B%E9%A1%B9" target="_blank" rel="noopener">这篇文章</a>。



## 关于 Web 题目环境

为了避免选手之间互相干扰，Web 题目为每个选手分配一个独立的后端环境。

点击 “打开/下载题目” 时，将会访问启动器（网址形如 <code>prob\*\*-manager.geekgame.pku.edu.cn/docker-manager/start?...</code>），这个系统将启动一个该题的后端环境（域名形如 <code>prob\*\*-xxxxxxxx.geekgame.pku.edu.cn</code>），并将选手重定向过去。

<b style="color: darkred">持续 15 分钟没有人访问时，后端环境将自动关闭。</b>下次点击 “打开/下载题目” 时会重新启动一个新的环境。

如果环境出现问题，选手可以将启动器网址的 <code>/start</code> 改成 <code>/stop</code> 来手动关闭题目。手动关闭后需要等待一分钟方可重新启动。

<b style="color: darkred">所有 Web 题目环境均没有外网，</b>因此无法反弹 Shell。



## 常用工具

下面列举了一些解决题目可能需要用到的工具，供选手参考。有些软件是收费的商业软件。

- 好用的编程语言，例如 Python
- 好用的搜索引擎，例如 Google 和 Bing
- 虚拟化软件，例如 VMWare Workstation、WSL 2、Docker 和 Wine
- （Misc）分析多媒体文件的工具，例如 Adobe 系列软件和 StegSolve
- （Misc 和 Web）分析二进制文件的工具，例如 010 Editor
- （Misc 和 Web）流量分析工具，例如 WireShark 和 Fiddler Classic
- （Web）HTTP 请求库，例如 requests
- （Web）现代浏览器的开发者工具，例如 Chrome DevTools
- （Binary）反编译器，例如 IDA (Hex-Rays Decompiler)、Ghidra 和 Binary Ninja
- （Binary）调试器，例如 gdb、IDA、pwndbg 和 x64dbg
- （Binary 和 Algorithm）与终端交互的库，例如 pwntools
- （Algorithm）科学计算和密码学算法库，例如 gmpy2、sympy 和 pycryptodome