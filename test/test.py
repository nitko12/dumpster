import sys
from tqdm import tqdm

import socket

import random
import string

HOST = "127.0.0.1"
PORT = 8080

len_ = 1000000

data = "".join(string.ascii_letters[random.randint(0, 25)] for _ in range(len_)).encode()

def recvall(sock, size):
    received_chunks = []
    buf_size = 4096
    remaining = size
    while remaining > 0:
        received = sock.recv(min(remaining, buf_size))
        if not received:
            raise Exception('unexpected EOF')
        received_chunks.append(received)
        remaining -= len(received)
    return b''.join(received_chunks)

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))

    progress = tqdm()
    while True:
        progress.update(1)

        s.sendall(data)

        recv = recvall(s, len_)

        if data != recv:
            with open("error.txt", "w") as f:
                f.write(data.decode())
                f.write("\n")
                f.write(recv.decode())
                f.write("\n")
            exit(1)

        # print(data)
