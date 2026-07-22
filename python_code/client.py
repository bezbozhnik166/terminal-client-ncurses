import socket

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

server.connect(('127.0.0.1',3000))

payload = "hello from client".encode()

server.sendall(payload)
