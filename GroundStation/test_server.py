# -*- coding: utf-8 -*-

import sys,socket,struct,time

if __name__=='__main__':
    s=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(('192.168.7.1',8000))
    while True:
        #connection, address = s.accept()
        recv_bytes=s.recv(9)
        head=struct.unpack("BBBBBBBBB", recv_bytes)
        print head
