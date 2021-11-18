import socket
import securesocket
import threading
import struct
import time
import libtreasure as lib
from Crypto.PublicKey import RSA

import logger
from flag import checktoken

BIND_ADDR = ('0.0.0.0', 10010)
SERVER_PRIVKEY = RSA.import_key('''
-----BEGIN RSA PRIVATE KEY-----
MIICXgIBAAKBgQDBiXYTdi8TOGnbitqe7d/7mzuH0qJ1y37r0eZznjtEJLwab6UT
3cxe1D0qnfr/mzGPnebjTHFaxf+XXPuBxNfSWXSt2n3DC/iFSoiLH+sYyLOkwMkk
5O1TjlLyFN6hrIY8dVPT14pEO+Dayu4dd33IZo1giVnNRv9fnW41pVQ2mQIDAQAB
AoGAEgCHSygjGNaWgiVYUJAv1WeLEgc1tV9IXwcXOInvWVdRZ+Qaps5qxCdZyS0I
u93jrkgxwdLKeHcSXzH28wssR+s3FP4j5RLIsYCGcNWCYZM/ENKk9M7ukdUoU6sN
crxX6wkWlg11kIyFPIxyQgioHKBeRQn/6UCS7y8Rmr4H3QECQQDU8PaXJN+vPlKn
pB8H+sprCrLrvGThXY73FTV8KvcOdVtjw4IsnKSOCRFLTMfkRbIw0+Wy+Iam9P4q
+exAgKwlAkEA6KwFqG7tKRVl/JhwS4qMniMP/yNIP+RlsbDvhStcQ5TvzvnYjYxH
kg0o1weHgqrDNURW4Ow51qA6ZE+XWfdcZQJBAKp5esEIGTffyiZBw0INKk5F03Xc
blrqHtF6nkYY0480rwSQeb+jbC313m2NO55pmb1zdrowMwSosG7RhK0jFi0CQQCy
l1fAcWSYpFQkDt4jHbXRrBgbPnm2kRl9ytIO5blsJmXr+Nq/y20kLfWrF5xsL6am
vE3zV18N3u7zECScacIBAkEA0yezWcnMDAO0okL2+g42o2A83ZTQq0y0KCo2Pb+V
iHQl7t7mAd9N0669BfJ1C2mfSuwFPaw8tAJi3ymWM9logQ==
-----END RSA PRIVATE KEY-----
'''.strip())
PSK = b'\x9a6\xfa\x8e\xcf\x82\x15\xd5\x0c\xc5\xa7\xa1\xb9\xd0 28,zO\x1f3!\xa1\xb0K\x9a\xe8\xea*\x15\xc1'

sbind = socket.socket()
sbind.bind(BIND_ADDR)
sbind.listen()

uidmap_lock = threading.Lock()
uidmap = {}

def reject(uid, secsock, text):
    logger.write(uid, ['reject', secsock.sock.getpeername(), text])
    try:
        secsock.send(struct.pack('!b', lib.Sync.show_hud))
        secsock.send(text.encode('utf-8'))
        time.sleep(.2)
    except Exception:
        pass
    print('REJECTED', text)
    secsock.sock.shutdown(socket.SHUT_RDWR)
    secsock.sock.close()

def worker(sock, info):
    secsock = securesocket.SecureSocket(sock, PSK, SERVER_PRIVKEY, None)

    token = secsock.recv(1024).decode('utf-8', 'replace')
    plat = secsock.recv(512).decode('utf-8', 'replace')
    uid = checktoken(token)
    logger.write(uid, ['connected', info, plat])
    
    if not uid:
        reject(uid, secsock, '验证失败')
        return
        
    game = lib.Game(secsock, token)
    game.plat = plat
    with uidmap_lock:
        if uid in uidmap:
            reject(uid, uidmap[uid].socket, '账号在其他设备登录：\n'+uidmap[uid].plat)
        uidmap[uid] = game
    
    try:
        secsock.sock.settimeout(180)
        game.start_controller()
    except (ConnectionResetError, RuntimeError):
        pass
    finally:
        with uidmap_lock:
            if uid in uidmap and uidmap[uid] is game:
                print('LEAVING', uid, info)
                logger.write(uid, ['leaving', info])
                del uidmap[uid]
            else:
                print('DROPPED', uid, info)
                logger.write(uid, ['dropped', info])

logger.write(None, ['server_started'])
print('server started.')
while True:
    sock, info = sbind.accept()
    sock.settimeout(2)
    threading.Thread(target=worker, args=[sock, info], daemon=True).start()
    