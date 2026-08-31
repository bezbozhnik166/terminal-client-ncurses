#client.py

import socket
import sys
import threading

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

while running:
    payload = sys.stdin.readline()

    if payload == "!exit\n":
        conn.close()
        break;

    elif payload:
        conn.sendall(payload.encode())
        payload = ""

print("test")
conn.close()
