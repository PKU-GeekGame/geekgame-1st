# uncompyle6 version 3.8.0
# Python bytecode 3.8.0 (3413)
# Decompiled from: Python 3.10.0 (tags/v3.10.0:b494f59, Oct  4 2021, 19:00:18) [MSC v.1929 64 bit (AMD64)]
# Embedded file name: securesocket.pyc
import socket, struct
from Crypto.PublicKey import RSA
from Crypto.Cipher import ChaCha20, PKCS1_OAEP
from Crypto.Random import get_random_bytes
import zlib

def apply_psk(psk, b):
    assert len(psk) == len(b)
    out = []
    for i in range(len(b)):
        out.append(b[i] ^ psk[i])
    else:
        return bytes(out)


class SecureSocket:

    def __init__(self, sock, psk, self_privkey, peer_pubkey):
        self.sock = sock
        self.sock_r = sock.makefile('rb')
        self.send_key = get_random_bytes(32)
        self.recv_key = self.handshake(self_privkey, peer_pubkey)
        self.send_key = apply_psk(psk, self.send_key)
        self.recv_key = apply_psk(psk, self.recv_key)
        self.send(b'HOLA')
        assert self.recv(4) == b'HOLA'
        print('handshake done')

    def send(self, b):
        assert isinstance(b, bytes)
        nonce = get_random_bytes(12)
        size = len(b)
        use_compression = size >= 16
        if use_compression:
            b = zlib.compress(b)
            size = len(b)
        size = size * 2 + (1 if use_compression else 0)
        b = ChaCha20.new(key=(self.send_key), nonce=nonce).encrypt(b)
        self.sock.sendall(struct.pack('!i', size) + nonce + b)

    def recv(self, maxsize):
        try:
            size = struct.unpack('!i', self.sock_r.read(4))[0]
            if size < 0:
                raise RuntimeError('connection is closed')
            use_compression = size & 1 != 0
            size = size // 2
            if maxsize is not None:
                assert size <= maxsize + 64, 'maxsize %s but got %s' % (maxsize, size)
            nonce = self.sock_r.read(12)
            b = self.sock_r.read(size)
            b = ChaCha20.new(key=(self.recv_key), nonce=nonce).decrypt(b)
            if use_compression:
                b = zlib.decompress(b)
            return b
        except socket.timeout as e:
            try:
                self.sock.send(struct.pack('!i', -1))
                self.sock.shutdown(socket.SHUT_RDWR)
                self.sock.close()
                raise RuntimeError('timeout') from e
            finally:
                e = None
                del e

    def handshake(self, self_privkey, peer_pubkey):
        if self_privkey is None:
            self_privkey = RSA.generate(1024)
        # else:
        self.sock.sendall(b'HELO')
        assert self.sock_r.read(4) == b'HELO'
        self.sock.sendall(self_privkey.public_key().n.to_bytes(128, byteorder='big'))
        recv_pubkey = int.from_bytes(self.sock_r.read(128), 'big')
        if peer_pubkey is None:
            peer_pubkey = RSA.construct((recv_pubkey, 65537))
        else:
            assert peer_pubkey.n == recv_pubkey
        encrypted_key = PKCS1_OAEP.new(peer_pubkey).encrypt(self.send_key)
        self.sock.sendall(struct.pack('!H', len(encrypted_key)) + encrypted_key)
        key_len = struct.unpack('!H', self.sock_r.read(2))[0]
        peer_chachakey = PKCS1_OAEP.new(self_privkey).decrypt(self.sock_r.read(key_len))
        assert len(peer_chachakey) == 32
        return peer_chachakey


def connect(spec, psk, server_pubkey):
    s = socket.socket()
    s.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
    s.connect(spec)
    return SecureSocket(s, psk, None, server_pubkey)