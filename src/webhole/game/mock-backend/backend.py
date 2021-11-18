from sanic import Sanic
from sanic.response import text, json, redirect, file
import time

from mockers import mock_text, mock_replycount, mock_nicknames
from random import randrange

app = Sanic('MockingBackend')

HOST = '0.0.0.0'
PORT = 5000
WORKERS = 4

TODO_TEXT = '前面的区域以后再来探索吧！'
BASE_PID = 160000
PAGESIZE = 30
MAXPAGE = 20

def mock_single(pid):
    ts = int(time.time())
    
    return {
        "pid": str(pid),
        "hidden": "0",
        "text": mock_text(randrange(1, 8+1)),
        "type": "text",
        "timestamp": str(ts - 180 - 60*(BASE_PID-pid)),
        "reply": mock_replycount(pid),
        "likenum": randrange(0, 20),
        "extra": "0",
        "url": "",
        "hot": str(ts),
        "tag": None
    }

def mock_getlist(p):
    ts = int(time.time())
    p = p-1 # starts from 0

    if p>=MAXPAGE or p<0:
        return {
            "code": 0,
            "data": [],
            "timestamp": ts,
            "count": 0,
        }
    else:
        return {
            "code": 0,
            "data": [mock_single(BASE_PID-p*PAGESIZE-idx) for idx in range(PAGESIZE)],
            "timestamp": ts,
            "count": PAGESIZE,
        }

def mock_getcomment(pid):
    pid = int(pid)
    ts = int(time.time())

    if pid>BASE_PID or pid<=0:
        return {"code": 1, "msg": "没有这条树洞"}
    else:
        nicknames = mock_nicknames(pid)
        return {
            "code": 0,
            "data": [{
                "cid": str(8200000 + pid*500 + idx*5 + randrange(0, 5)),
                "pid": str(pid),
                "text": f'[{nicknames[idx]}] {mock_text(randrange(1, 3+1))}',
                "timestamp": str(ts - 180 - 60*(BASE_PID-pid) + idx*10 + randrange(0, 10)),
                "anonymous": "1",
                "tag": None,
                "islz": nicknames[idx]=='洞主',
                "name": nicknames[idx],
            } for idx in range(mock_replycount(pid))],
            "attention": 0,
            "captcha": False,
        }

def mock_search(term, p, ps):
    ts = int(time.time())

    if p!=1 or ps<=0 or not 1<=len(term)<=60:
        return {"code": 0, "data": []}
    else:
        pid = BASE_PID - MAXPAGE*PAGESIZE - randrange(100, 5000)
        return {
            "code": 0,
            "data": [{
                "pid": str(pid),
                "text": mock_text(randrange(5, 8+1), term),
                "type": "text",
                "timestamp": str(ts - 180 - 60*MAXPAGE*PAGESIZE - randrange(86400, 86400*240)),
                "reply": str(mock_replycount(pid)),
                "likenum": randrange(0, 20),
                "extra": "0",
                "url": "",
                "tag": None
            }]
        }

def mock_getone(pid):
    ts = int(time.time())
    
    if pid>BASE_PID or pid<10032:
        return {
            "code": 1,
            "msg": '没有这条树洞'
        }
    else:
        return {
            "code": 0,
            "data": mock_single(pid),
            "timestamp": ts,
        }

def mock_getattention():
    ts = int(time.time())

    return {
        "code": 0,
        "data": [],
        "timestamp": ts,
    }

@app.route('/mock_api/services/qkuhole/api.php')
async def hole_api(req):
    action = req.args.get('action')
    if action=='getlist':
        return json(mock_getlist(int(req.args.get('p'))))
    elif action=='getcomment':
        return json(mock_getcomment(int(req.args.get('pid'))))
    elif action=='search':
        return json(mock_search(req.args.get('keywords', ''), int(req.args.get('page')), int(req.args.get('pagesize'))))
    elif action=='getone':
        return json(mock_getone(int(req.args.get('pid'))))
    elif action=='getattention':
        return json(mock_getattention())
    else:
        #print('!!', action)
        return text(TODO_TEXT, status=501)

@app.route('/mock_api/api_xmcp/hole/system_msg')
async def sys_msg(req):
    return json({
        "error": None,
        "result": [{
            "content": TODO_TEXT,
            "timestamp": int(time.time())-10,
            "title": "Q·A·I·M·O·N"
        }],
    })

@app.route('/hole/', strict_slashes=True)
async def hole(req):
    return await file('build/index.html', mime_type='text/html')

@app.route('/')
async def index(req):
    return redirect('/hole/')

app.static('/hole', 'build', name='frontend-files')

@app.route('/<path:path>', methods=['GET', 'POST'])
async def fallback(req, path=None):
    return text(TODO_TEXT, status=501)

@app.on_response
async def add_cache_header(req, resp):
    resp.headers['Cache-Control'] = 'no-store, max-age=0'

@app.on_request
async def filter_map_file(req):
    if req.path.lower().endswith('.map'):
        return text('Forbidden', status=403)
    
if __name__ == '__main__':
    app.run(host=HOST, port=PORT, access_log=False, debug=False, workers=WORKERS)