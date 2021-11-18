#!/usr/bin/env python
#coding:utf-8

import socketserver
import base64, random


class ChatBotServer(socketserver.BaseRequestHandler):
    def handle(self):
        conn = self.request
        while True:
            data = conn.recv(2048).decode('utf8')
            if data.strip() == "exit":
                print("断开与%s的连接！" % (self.client_address,))
                conn.sendall((b"%s\n" % base64.b64encode('Goodbye!')))
                break
            r = [b'Hello!', b'Alola!', b'Nice to meet you.', b'What a wonderful game!', b'Try again and harder!', b'Good luck to you!']
            conn.sendall((b"%s\n" % base64.b64encode(random.choice(r))))
            break

if __name__ == '__main__':
    server = socketserver.ThreadingTCPServer(('127.0.0.1', 1234), ChatBotServer)
    print("启动ChatBot！")
    server.serve_forever()
