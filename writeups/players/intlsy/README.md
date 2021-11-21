id: intlsy

无条件公开，并且我非常希望我的 writeup 被公开。求求了。

# 密码学实践

为什么先讲这道题？因为这道题真的很有意思！

## Flag1

flag1 要求我们去破解 `MESenc` 这个函数。

观察这个函数，可以发现它干了以下几件事：

-   把 key 切成 32 块，每块含有 8 个 byte
-   把 mess 切成 $\operatorname{len}(mess)/32$ 块，每一块含有 32 个 byte。
-   对每一块分别加密，然后把加密结果拼起来

我们再来探究 mess 的每一块是怎么被加密的：

-   这 32 个 byte 又被分成了四个部分 $a,b,c,d$，经过一些魔法之后，得到了 $a',b',c',d'$。 

我们探究一下 $a',b',c',d',a,b,c,d$ 之间的关系：

```python
import os, sys
import base64

# 假设 32 个 key 分别是 A,B,C,D...,Y,Z,0,1,2,3,4,5
keys = [ chr(x) for x in range(ord('A'),ord('Z')+1) ] + [ str(x) for x in range(0,6) ]
assert len(keys) == 32
charset = keys + ["a","b","c","d"]

# 假设 a,b,c,d 的初始值分别是 a,b,c,d
a,b,c,d = "a","b","c","d"

# 模拟 MESenc 的过程
for key in keys:
    a,b,c,d = b,c,d,a+c+key
   
# 因为 a xor a = 0 并且异或满足交换律、结合律，所以我们可以把表达式化简
def Simplify(s):
    result = ''
    for c in charset:
        if s.count(c)%2 == 1:
            result += c
    return result
    
a = Simplify(a)
b = Simplify(b)
c = Simplify(c)
d = Simplify(d)
print(a,b,c,d)
```

输出为：`CEIKOQUW02c DFJLPRVX13d AEGKMQSWY24ac BFHLNRTXZ35bd`

也就是说，一个 key 可以使用四个数 $P_1,P_2,P_3,P_4$ 去刻画。我们不关心 key 的具体值，只关心 $P_1,P_2,P_3,P_4$ 的值就行了。加密完成后，

$$a' = P_1 \operatorname{xor} c\\b' = P_2 \operatorname{xor} d\\c' = P_3 \operatorname{xor} a \operatorname{xor} c\\d' = P_4 \operatorname{xor} b \operatorname{xor} d$$

发现这个规律之后，一切就很好搞了。我们可以通过 `"Sorry, I forget to verify your i"` 的加密结果算出 $P_1,P_2,P_3,P_4$ 的值，然后逆推出 flag1 的值。

## Flag2

首先，我们需要让 `aname = b'Alice'`

这一步我们就需要用到“Talk To God”了（我讨厌 God，下文称 God 为加密 Oracle）

我们的大体思路是：利用加密 Oracle 获得某个 byte 串 $S$ 加密后的数值，然后计算出另一个 byte 串 $T$ 加密后的数值。其中 $T$ 满足：$T$ 被“解包”之后得到的 `aname` 是 `b'Alice'`。

在经过了一上午的构造之后，我找到了以下这种构造方式：

我们先向加密 Oracle 中输入 `name=b''`（空串），`key=b'Alice'`。我们可以得到 `enc(b'Alice\x00\x05')` 的值。

我们的目标是得到 `enc(b'Alice\00\05\00\00')` 的数值。

不难发现，后者的值就是前者的值乘上 $65536^{rsa_d}$。（对了，别忘了对 $rsa_N$ 取模）

那么怎么获得 $65536^{rsa_d}$ 呢？我们向加密 Oracle 中输入 `name=b'\x00'`，`key=b'\x00\x00'` 即可得到 `enc(65536)` 的值。

done.

接下来的步骤就和 Flag1 一样了。

# 签到

下载题目中给的 pdf，看到最后两行有奇怪的字符。用 Google Chrome 打开 pdf，复制最后两行字符（注意要复制全！），得到：

```
fa{aeAGetTm@ekaev!
lgHv__ra_ieGeGm_1}
```

隐约出现了 flag 字样。把两串字符“你一个，我一个”地拼起来，得到最终 Flag

# 小北问答 Remake

>   北京大学燕园校区有理科 1 号楼到理科 X 号楼，但没有理科 (X+1) 号及之后的楼。X 是？

看地图即可。X = 5。

>   上一届（第零届）比赛的总注册人数有多少？

在大信科公众号里面搜索“第零届”即可。

>   geekgame.pku.edu.cn 的 HTTPS 证书曾有一次忘记续期了，发生过期的时间是？

https://crt.sh/。搜索 geekgame.pku.edu.cn 即可。

>   2020 年 DEFCON CTF 资格赛签到题的 flag 是？

google 搜索 DEFCON CTF QUAL 2020 即可。我当时打开了那场比赛的主页然后把签到题做了一遍。

>   在大小为 672328094 * 386900246 的方形棋盘上放 3 枚（相同的）皇后且它们互不攻击，有几种方法？

果断打开 OEIS，搜索 queens，在 [A047659](https://oeis.org/A047659) 的 FORMULA 节里面找到在 $m\times n$ 的棋盘上放 3 个皇后的方案数的通项公式。写成代码即可。注意不要使用浮点数（防止误差）。详见代码 `4/solve.py`

>   上一届（第零届）比赛的“小北问答1202”题目会把所有选手提交的答案存到 SQLite 数据库的一个表中，这个表名叫？

github 上找 geekgame 0th 的仓库，找到题目“小北问答”的源代码目录，打开 [game/db.py](https://github.com/PKU-GeekGame/geekgame-0th/blob/main/src/choice/game/db.py) ，看到

```python
db.executescript('''
        create table if not exists submits (
            uid int,
            submit_ts int,
            answers_json text
        );
    ''')
```

所以答案是 `submits`

>   国际互联网由许多个自治系统（AS）组成。北京大学有一个自己的自治系统，它的编号是？

google 搜索 `国际互联网 自治系统`，找到北京大学的 AS 即可。

>   截止到 2021 年 6 月 1 日，完全由北京大学信息科学技术学院下属的中文名称最长的实验室叫？

经过一番查找之后找到了：https://eecs.pku.edu.cn/info/1060/11528.htm

# 共享的机器

打开题目中给的 Etherscan 的链接，发现这东西是个智能合约。

使用 Etherscan 上的反编译器去反编译合约的 ByteCode，得到

```python
# 删掉了一些用处不大的代码
# 对照着 https://ethervm.io/decompile 的反编译结果，修正了一些错误
def storage:
  stor2 is uint256 at storage 2
  stor3 is uint256 at storage 3
def unknownded0677d(uint256 _param1) payable:
  idx = 0
  s = 0
  while idx < 64:
      idx = idx + 1 # 这里反编译错了，idx = idx+1 应该在循环的末尾
      s = s or (Mask(256, -4 * idx, _param1) >> 4 * idx) + (5 * idx) + (7 * Mask(256, -4 * idx, stor2) >> 4 * idx) % 16 << 4 * idx
        # 这里面的 or 表示的是 bitwise or
  if stor3 != 0: # 这里应该改成 stor3 != s
      revert with 0, 'this is not the real flag!'
  return 1
```

我们的目标（大概应该可能）就是让 `unknownded0677d` 这个函数返回 1。

首先，我们需要在 Etherscan 上通过交易记录找到 `stor2` 和 `stor3` 的值。

其次，我们需要去寻找一个 `param1` 使得 `unknownded0677d(param1)` 返回 1。

我们只要照着 `unknownded0677d` 的逻辑，逆向地、从高位到低位地逐渐构造 `param1` 即可。

详见代码 `6/solve.py`

# 翻车的谜语人

下载那个 pcap 文件，用 wireshark 打开，发现有很多 http 包和 websocket 包。

首先筛选 http 包，发现了很多 `ipynb` 字样，判断 You 酱在用 Jupiter Notebook。

~~我还真没注意到他在挂 B 站直播间~~

找到了几个特殊的包：

```
3161 python 脚本 把 encoded_flag2 写到了 flag2.txt，还有一个叫 key 的变量的值
8876 python 脚本 把 encoded_flag1 写到了 flag1.txt
9102 列出目录，表示当时目录中存在 ki-ringtrain.wav, Untitled.ipynb, flag1.txt, flag2.txt
10641 edit flag1.txt
11265 flag1.txt 的内容
13999 一个奇怪的 POST，地址是 /api/terminals
45021 列出目录，表示当时目录中存在 flag2.wav, ki-ringtrain.wav, Untitled.ipynb, flag2.7z, flag1.txt, flag2.txt
47395 flag2.7z 的内容社交牛逼症）
```

只要把这些包都找到了，这道题就很好做了。

## Flag1

打开 8876 号包，找到用于生成 flag1.txt 的 python 脚本，发现 flag1.txt 的生成方式大概是 flag1 xor key，其中 xor 代表按位异或。从 3161 号包中可以获取到 key 的值，从 11265 号包中可以获取到 flag1 xor key 的值。依此可以还原 flag1。

还原 flag1 的代码见 `7/restor1.py`

## Flag2

从 47395 号包中可以获取到 flag2.7z。尝试解压，发现有密码。

将筛选规则改为 `http || websocket`，发现从 14340 号包开始有一大堆 websocket 包。查看其内容，不难发现是 You 酱在使用终端。大概执行了以下命令（按记忆写的，不一定完全正确）

```bash
pip install stego-lsb
stegolsb wavsteg -i ki-ringtrain.wav -s flag2.txt -o flag2.wav
7za a flag2.7z flag2.wav -p"Wakarimasu! `date` `uname -nom` `nproc`"
```

说明，You 酱大概是先用 stegolsb 把 flag2.txt 隐写到了 flag2.wav 里面，然后压缩。密码是 Wakarimasu! `date` `uname -nom` `nproc`。

由 terminal 的 prompt 知道 `uname -nom` 是 `you-kali-vm x86_64 GNU/Linux`。由 7zip 的输出知道 `nproc` 是 `8`。装一个 kali 虚拟机，调整时区为中国之后，得到 `date` 的输出是 `Sat 06 Nov 2021 03:44:15 PM CST`。组合起来，得到密码，解压缩后用 `stegolsb` 还原即可。

暴力尝试密码的代码见 `7/crackPassword.py`。这个代码本质上就是在枚举不同的 `date`, `uname` 和 `nproc` 的组合，逐一尝试。

# 在线解压网站

观察网站源代码，发现网站的功能极其简单：解压缩并提供“下载解压缩后的文件”的功能。而且网站没有对目录做任何检查。

尝试在 URL 中加入 `../` 或者类似的东西，发现行不通。~~也可能是因为我太菜~~

于是在压缩包里面装了一个指向 `/flag` 的符号连接，压缩，上传，下载，~~woc 居然~~成功了。

# 早期人类聊天室

点击 `Open Source` 拿到源代码，发现 `module()` 函数内并没有对 `logfile` 的路径做任何检查。

于是尝试去篡改 `https://prob17-qgcy7n8a.geekgame.pku.edu.cn/module?name=chatlog&log=chat_log_2021_11_20` 中的 log 参数，得到了以下文件（这里只列出了之后会用到的文件）

```
../run.sh
../../../../etc/supervisor-ctf.conf
../../../../tmp/uwsgi-ctf.ini
../../../../tmp/uwsgi.pid
../../../../tmp/supervisor.log
```

观察可以发现：

-   flag 位于 `/flag`，但这个文件的权限是 600（rw-------），只有 root 能够读取。也就是说，我们需要获取 root 身份
-   supervisor 使用 root 身份启动了 uwsgi，但是 uwsgi.ini 中设置了 uid=nobody

~~然后我就不会了 QAQ，等第二阶段~~

打开第二阶段的提示，看到了那个“远程代码执行漏洞”，下载脚本，魔改之。魔改后的脚本见 `17/uwsgi-exp.py`。

我们先理一下思路：

-   为了获取 flag，我们需要获取到 root 权限
-   为了获取 root 权限，我们需要以 root 的身份启动 uwsgi
-   为了以 root 的身份启动 uwsgi，我们需要改掉 uwsgi 的配置文件，并让 uwsgi 重新加载配置文件

那么目标很清晰：

-   改掉 uwsgi 的配置文件
-   让 uwsgi 重新加载配置文件

## 改掉 uwsgi 配置文件

```bash
echo \"[uwsgi]
socket = :3031
chdir = /usr/src/ufctf
manage-script-name = true
mount = /=app:app
master = true
uid = root
gid = root
workers = 1
buffer-size = 65535
enable-threads = false
pidfile = /tmp/uwsgi-2.pid
\" > /tmp/uwsgi-ctf.ini
```

用魔改 `uwsgi-exp.py` 获取其 payload，提交即可。

## 重启 uwsgi

```bash
uwsgi --stop /tmp/uwsgi.pid
```

emmm 为啥不是 `uwsgi --reload /tmp/uwsgi.pid`？我试了一下，直接用 `uwsgi --reload /tmp/uwsgi.pid` 或者把 uwsgi 的 pid 直接 kill 掉的话 uwsgi 还是会以 nobody 的身份运行。原因我也不太懂，等官方 wp。

然后你就可以快乐地访问 `/flag` 了。

# Flag 即服务

## Flag1

观察 `https://prob11-86aqylgw.geekgame.pku.edu.cn/api/demo.json`，发现这很像是个“返回磁盘上文件”的操作，于是构造 `https://prob11-86aqylgw.geekgame.pku.edu.cn/api/..%2F/package.json`，得到：

```json
{"name":"demo-server","version":"1.0.0","description":"","scripts":{"start":"node --max-http-header-size=32768 start.js"},"author":"You","license":"WTFPL","dependencies":{"jsonaas-backend":"https://geekgame.pku.edu.cn/static/super-secret-jsonaas-backend-1.0.1.tgz"}}
```

得到装着网站源代码的压缩包。打开源代码，发现

```javascript
if(FLAG0!==`flag{${0.1+0.2}}`)
```

~~不会有人真的和我一样下意识地认为 0.1+0.2 = 0.3 吧~~把它丢进 nodejs 运行，发现结果是 `flag{0.30000000000000004}`

 后面两个 Flag 我都不会 QAQ

# 诡异的网关

探索 UI，可以发现存在一个 UserId 是 flag 的用户，而且保存了密码。我们猜测密码就是 Flag。

使用 Microsoft spy++ 即可获得密码输入框中的内容。done.

# 最强大脑

## Flag1

我在第二阶段才拿到了这个 Flag。

打开第二阶段新增的附件 `bf.c`，阅读源代码。观察到这一段：

```c
void realloc_data(){
	uint64_t pos = data_ptr - data;
	uint64_t new_data_len = data_len * 2;
	if (new_data_len < MIN_DATA_LEN)
		new_data_len = MIN_DATA_LEN;
	data = realloc(data, new_data_len);
	memset(data + data_len, 0, new_data_len - data_len);
	data_len = new_data_len;
	data_ptr = data + pos;
}
/* 读取 flag1，并把 flag1 存在 data 的最末尾的位置 */
void put_flag1(){
	FILE *f = fopen("flag1", "r");
	fseek(f, 0, SEEK_END);
	int flag_len = ftell(f);
	fseek(f, 0, SEEK_SET);
	char *flag = malloc(flag_len);
	fread(flag, 1, flag_len, f);
	fclose(f);
	memcpy(data + data_len - flag_len, flag, flag_len);
	free(flag);
}
int main(){
    ...
	realloc_data();	// 把初始的 data 数组给 alloc 好
	put_flag1();
    ...
}
```

可以发现，程序先把 data 数组的大小扩大成了 `MIN_DATA_LEN`(=4096)，然后把 flag1 放到了数组最末尾的位置。

所以只需要构造一下 brainf**k 程序：

```
（x 个 >）.
```

$x$ 依次取 $4095$, $4094$, $4093$...

这样就可以把整个 flag 给读出来了。

# 扫雷

我只会做 Flag1

google 搜索 python random cracker，找到了[这个项目](https://github.com/tna0y/Python-random-module-cracker)。阅读 `README.md` 可以知道，我们只需要获得随机数生成器生成的前 624 个 32 位整数的值，我们就可以预测随机数生成器的行为。

在没有开启 `easy_mode` 的情况下，我们可以很容易地从 BOOM! 之后的棋盘数据中获得这些值。

代码详见 `9/simulator.py`

# 龙珠模拟器

我只会做 Flag1

观察程序发现，Basic 模式下，小球生成时的参数相同（除了 chunkSize）。

所以，只要锁定 `chunkOffsetX = 0, chunkOffsetY = 0`，就可以使生成每种球的时候，随机数生成器的种子相同。

阅读 `java` 中 `Random` 的源代码，探索一下 `nextInt` 的实现方式。发现 `nextInt` 大概率返回 `next(31) % bound`。

再次观察每种球的 chunkSize，发现它们呈倍数关系。

也就是说：（在 `chunkOffsetX = 0, chunkOffsetY = 0` 的区块内）第 $i+1$ 种球的坐标模上 $chunkSize_i$ ，就能得到第 $i$ 中球的坐标。

换句话说，第 $i+1$ 种球的坐标一定是第 $i$ 中球的坐标的整数倍。这可以大大降低枚举量。

枚举即可。

代码：`16/solve.py`

