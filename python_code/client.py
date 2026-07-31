import socket
import sys

conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

conn.connect(('127.0.0.1',3000))

# payload = "hello from client".encode()

running = 1

while running:
    payload = sys.stdin.readline().encode()

    if payload:
        conn.sendall(payload)
        payload = ""

conn.close()
