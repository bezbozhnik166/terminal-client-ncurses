import socket as s
import threading

HOST, PORT = '127.0.0.1', 3000

server = s.socket(s.AF_INET, s.SOCK_STREAM)

# server.setsockopt(s.SOL_SOCKET, s.SO_REUSEADDR, 1)

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
                print(f"got message: {data.decode()}",end="")
                self.broadcast(data)

            else: # I'm trying to close the thread for the client after they disconnect but for some reason it doesn't seem to work. What am i doing wrong ?
                print("a user has disconnnected")
                active_clients.remove(self.conn)
                self.conn.close()
                self.running = False


server.listen()

print(f"[Starting] Starting server on {HOST}:{PORT}")

try:
    while True:
        conn, addr = server.accept()
        new_user = User(conn,addr)
        thread = threading.Thread(target=new_user.handle_client)
        thread.start()

        print(f"[ACTIVE] {threading.active_count()- 1}")
except KeyboardInterrupt:
    print("exiting")

finally:
    server.close()
