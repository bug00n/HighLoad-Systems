import socket
import threading
import time

HOST = "127.0.0.1"
PORT = 8000
CLIENTS = 1000
MESSAGES_PER_CLIENT = 30

def client_worker():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        for i in range(MESSAGES_PER_CLIENT):
            s.sendall(f"Hello {i}\n".encode())
            data = s.recv(1024)


threads = []
for _ in range(CLIENTS):
    t = threading.Thread(target=client_worker)
    threads.append(t)

first = time.time()

for t in threads:
    t.start()

for t in threads:
    t.join()

second = time.time()

print(f"Load test completed in {second - first}s")
