#server.py

import socket as s
import threading

HOST, PORT = '127.0.0.1', 3000

server = s.socket(s.AF_INET, s.SOCK_STREAM)

# server.setsockopt(s.SOL_SOCKET, s.SO_REUSEADDR, 1)

server.bind((HOST,PORT))

active_clients = []

server_closing = threading.Event()

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
        self.conn.settimeout(1.0)
        while self.running and not server_closing.is_set():
            try:
                data = self.conn.recv(1024)

                if data:
                    print(f"got message: {data.decode()}",end="")
                    self.broadcast(data)

                else:
                    print("a user has disconnnected")
                    active_clients.remove(self.conn)
                    self.conn.close()
                    self.running = False

            except s.timeout:
                continue

server.listen()

threads = []

print(f"[Starting] Starting server on {HOST}:{PORT}")

try:
    while True:
        conn, addr = server.accept()
        new_user = User(conn,addr)
        thread = threading.Thread(target=new_user.handle_client) #kill the threads while closing
        threads.append(thread)
        thread.start()

        print(f"[ACTIVE] {threading.active_count()- 1}")
except KeyboardInterrupt:
    print("exiting")

finally:
    server_closing.set()
    server.close()

    for thread in threads:
        thread.join()

    print("All threads stopped")
