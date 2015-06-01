import socket
import sys
import struct
import threading

class ServerSocket:
    def __init__(self, address, queue=5):
        self.sock=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock.bind(address)
        self.sock.listen(queue)
        return

    def accept(self):
        client, (host, port)=self.sock.accept()
        self.client=client
        self.address=(host, port)
        return (client, (host, port))

    def recv(self, le):
        msg=b''
        while len(msg)<le:
            chunk=self.client.recv(le-len(msg))
            if chunk==b'':
                raise RuntimeError("socket connection broken")
            msg+=chunk
        return msg

    def sendall(self, byte):
        self.client.sendall(byte)
        return

    def close(self):
        self.client.close()

    def shutdown(self):
        self.sock.close()

    def listen(self, n):
        self.sock.listen(n)
        

if __name__=='__main__':
    host=''
    port=[i for i in range(10001,10002)]
    missions=[]

    stop_server=False

    def do_server(ho, port, queue):
        server_sock=ServerSocket((ho, port), queue)
        server_sock.accept()

        while not stop_server:
            pa=server_sock.recv(4)
            i,=struct.unpack('!i', pa)
            print("integer: %d"%(i))
            b=server_sock.recv(i)

        return
                
    for p in port:
        missions.append(threading.Thread(target=do_server, args=(host, p, 1)))

    for m in missions:
        m.daemon=True
        m.start()

    print("hello world")

    def end_server():
        stop_server=True
