import socket as s
import threading

HOST, PORT = '127.0.0.1', 3000

server = s.socket(s.AF_INET, s.SOCK_STREAM)

server.bind((HOST,PORT))

active_clients = []

class User:
    def __init__(self,conn,addr):
        self.registered = False
        self.username = ''
        self.conn = conn
        self.addr = addr
        self.running = True

    def broadcast(self,data):
        for client in active_clients:
            if client != self.conn:
                client.sendall(data)

    def register(self):
        active_clients.append(self.conn)

    def handle_client(self): #main entry point of the program
        self.register()
        while self.running:
            data = self.conn.recv(1024)

            if data:
                print(f"got message: {data.decode()}")
                self.broadcast(data)


server.listen()

print(f"[Starting] Starting server on {HOST}:{PORT}")

while True:
    conn, addr = server.accept()
    new_user = User(conn,addr)
    thread = threading.Thread(target=(new_user.handle_client))
    thread.start()

    print(f"[ACTIVE] {threading.active_count()- 1}")
