#client.py

import socket
import sys
import threading
import struct

TYPE_LOGIN = 1
TYPE_CHAT = 2

conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

conn.connect(('127.0.0.1',3000))

# payload = "hello from client".encode()

running = 1

def recv_from_server():
    while True:
        data = conn.recv(1024) 
        sys.stdout.write(data.decode())
        sys.stdout.flush()
        if not data:
            break

thread = threading.Thread(target=recv_from_server)
thread.start()

first_payload = True
while running:
    payload = sys.stdin.readline()

    if payload == "!exit\n":
        conn.close()
        break;

    elif payload:
        if first_payload == True:
            username, password = payload.strip().split(',')
            payload = f"{username}\0{password}".encode()
            header = struct.pack("!II", TYPE_LOGIN, len(payload))
            conn.sendall(header)
            conn.sendall(payload)
            payload = ""
            first_payload = False
        else:
            header = struct.pack("!II", TYPE_CHAT, len(payload))
            conn.sendall(header)
            conn.sendall(payload.encode())
            payload = ""

print("test")
conn.close()
