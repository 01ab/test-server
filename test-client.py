import socket, json

def test_connection():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(1)
    s.connect(("localhost", 2020))
    data = {'request':'database'}
    s.sendall(json.dumps(data).encode())
    print(s.recv(5000))
    s.close()

for i in range(1004): 
    test_connection()