import socket, json

def test_connection():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(1)
    s.connect(("localhost", 2020))
    data = {'request':'database'}
    s.sendall(json.dumps(data).encode())
    print(s.recv(15000).decode())
    s.close()

for i in range(4): 
    test_connection()