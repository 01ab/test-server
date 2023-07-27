import socket

def test_connection():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(1)
    s.connect(("localhost", 2020))
    s.sendall(b"test")
    print(s.recv(5000))
    s.close()

for i in range(10004): 
    test_connection()