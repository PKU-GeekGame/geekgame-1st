import socket
from time import sleep
from datetime import date
from pprint import pformat

def get_today():
    return date.today().strftime('%Y_%m_%d')

def var_dump(var):
    return pformat(var)

def get_default_server():
    return '127.0.0.1:1234'

def send_msg(server_addr, msg):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        (ip, port) = server_addr.strip().split(':')
        s.connect((ip, int(port)))
        s.sendall(msg)
        sleep(1)
        r = s.recv(2048)
        s.close()
        return r.decode('utf8')
    except Exception as e:
        s.close()
        return str(e)

def write_chat_log(send, recv):
    logfile = open('media/chat_log_{}'.format(get_today()), 'a+')
    logfile.writelines(['you sent --->\n', send+'\n', 'server replied <---\n', recv+'\n'])
