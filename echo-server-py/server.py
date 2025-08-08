import socket as sock

socket = sock.socket(sock.AF_INET, sock.SOCK_STREAM | sock.SOCK_NONBLOCK) # ipv4, nonblocking
address = ('0.0.0.0', 8080) # server will work on all available interfaces with port 8080

socket.setsockopt(sock.SOL_SOCKET, sock.SO_REUSEADDR, True) # allow reuse of socket

socket.bind(address)
socket.listen(sock.SOMAXCONN)

BUFFER_SIZE = 1024

while True:
    try:
        client, addr = socket.accept()
        txt = client.recv(BUFFER_SIZE)
        if txt:
            print(f"Received: {str(txt, 'utf-8')}", end='')
            client.sendall(txt)
        client.close()
    except BlockingIOError:
        pass # there is no connections to accept