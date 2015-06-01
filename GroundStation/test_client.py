# -*- coding: utf-8 -*-

import sys,socket,struct,time

if __name__=='__main__':
    s=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(('192.168.7.1',8008))
    #i=5
    while True:
        #i=i-1
        time.sleep(0.4)
        send_bytes=struct.pack("@BBBBiiiiiiiiiiiiiiiiiiiiiiiiiffffff",\
                                0xff,0xaa,0xbb,0xcc,120,0,1,2,3,4,5,6,7,8,9,\
                                0,1,2,3,4,5,6,7,8,9,0,1,2,3,411111111,511111111,611111111,711111111,811111111,911111111)
        print send_bytes
        t=struct.unpack("BBBBiiiiiiiiiiiiiiiiiiiiiiiiiffffff",send_bytes)
        print t
        s.sendall(send_bytes)
        print('sended!')

        time.sleep(1)
        send_bytes=struct.pack("@BBBBiiiiiiiiiiiiiiiiiiiiiiiiiffffff",\
                                0xff,0xaa,0xbb,0xcc,120,10,11,12,13,14,15,16,17,18,19,\
                                10,11,12,13,14,15,16,17,18,19,10,11,12,13,14,15,16,17,18,19)
        print send_bytes
        t=struct.unpack("BBBBiiiiiiiiiiiiiiiiiiiiiiiiiffffff",send_bytes)
        print t
        s.sendall(send_bytes)
        print('sended!')

