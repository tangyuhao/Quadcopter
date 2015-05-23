# -*- coding: utf-8 -*-

import sys
import socket
import threading
import paramiko
import struct
import pygame
import time
import vlc

from PyQt4 import QtCore, QtGui
from Ui_main_copy import *
from Server import *
#from qtvlc import Player

global throttle,roll,pitch,yaw,mode,ctrl
throttle=0x44C #1100
roll=0x5DC #1500
pitch=0x5DC #1500
yaw=0x5DC #1500
mode=0x1
ctrl=0x2

global joystick,joystick_nameoutput
joystick=0
joystick_nameoutput=0

send_sock=None

ui=None

class main(QtGui.QDialog, Ui_Form):#, Player
    def __init__(self, parent=None):
        QtGui.QDialog.__init__(self, parent)
        self.setupUi(self)
        #self.createUI()
        
    @QtCore.pyqtSlot(int)
    def on_slider_throto_valueChanged(self, value):
        self.label_throto.setText(str(value))
        global throttle
        throttle=value
        #send_CHANNEL(throttle,roll,pitch,yaw,mode)
    
    @QtCore.pyqtSlot(int)
    def on_slider_roll_valueChanged(self, value):
        self.label_roll.setText(str(value))
        global roll
        roll=value
        #send_CHANNEL(throttle,roll,pitch,yaw,mode)
    
    @QtCore.pyqtSlot(int)
    def on_slider_pitch_valueChanged(self, value):
        self.label_pitch.setText(str(value))
        global pitch
        pitch=value
        #send_CHANNEL(throttle,roll,pitch,yaw,mode)
    
    @QtCore.pyqtSlot(int)
    def on_slider_yaw_valueChanged(self, value):
        self.label_yaw.setText(str(value))
        global yaw
        yaw=value
        #send_CHANNEL(throttle,roll,pitch,yaw,mode)
    
    @QtCore.pyqtSlot()
    def on_radioButton_loiter_clicked(self):
        self.textBrowser.append('Mode: loiter')
        self.textBrowser.moveCursor(QtGui.QTextCursor.End)
        global mode
        mode=0x3  #loiter
        #send_CHANNEL(throttle,roll,pitch,yaw,mode)
    
    @QtCore.pyqtSlot()
    def on_radioButton_auto_clicked(self):
        self.textBrowser.append('Mode: auto')
        self.textBrowser.moveCursor(QtGui.QTextCursor.End)
        global mode
        mode=0x5  #auto
        #send_CHANNEL(throttle,roll,pitch,yaw,mode)
    
    @QtCore.pyqtSlot()
    def on_radioButton_land_clicked(self):
        self.textBrowser.append('Mode: land')
        self.textBrowser.moveCursor(QtGui.QTextCursor.End)
        global mode
        mode=0x2  #land
        #send_CHANNEL(throttle,roll,pitch,yaw,mode)
    
    @QtCore.pyqtSlot()
    def on_radioButton_stablize_clicked(self):
        self.textBrowser.append('Mode: stablize')
        self.textBrowser.moveCursor(QtGui.QTextCursor.End)
        global mode
        mode=0x1  #stablize
        #send_CHANNEL(throttle,roll,pitch,yaw,mode)
    
    @QtCore.pyqtSlot()
    def on_radioButton_altitude_clicked(self):
        self.textBrowser.append('Mode: altitude')
        self.textBrowser.moveCursor(QtGui.QTextCursor.End)
        global mode
        mode=0x4  #alt_hold
        #send_CHANNEL(throttle,roll,pitch,yaw,mode)

    @QtCore.pyqtSlot()
    def on_pushButton_level_clicked(self):
        self.textBrowser.setText('Restart...level')
        self.textBrowser.moveCursor(QtGui.QTextCursor.End)
        global ctrl
        ctrl=0xa  #level
        #send_CONTROL(ctrl)

    @QtCore.pyqtSlot()
    def on_pushButton_arm_clicked(self):
        self.textBrowser.append('CTRL: arm')
        self.textBrowser.moveCursor(QtGui.QTextCursor.End)
        global ctrl
        ctrl=0xb  #arm
        #send_CONTROL(ctrl)

    @QtCore.pyqtSlot()
    def on_pushButton_disarm_clicked(self):
        self.textBrowser.append('CTRL: disarm')
        self.textBrowser.moveCursor(QtGui.QTextCursor.End)
        global ctrl
        ctrl=0xc  #disarm
        #send_CONTROL(ctrl)

    @QtCore.pyqtSlot()
    def on_pushButton_takeoff_clicked(self):
        self.textBrowser.append('CTRL: takeoff')
        self.textBrowser.moveCursor(QtGui.QTextCursor.End)
        global ctrl
        ctrl=0xd  #takeoff
        #send_CONTROL(ctrl)

    @QtCore.pyqtSlot()
    def on_pushButton_connect_clicked(self):
        self.textBrowser.append('Begin ssh')
        self.textBrowser.moveCursor(QtGui.QTextCursor.End)
        ssh = paramiko.SSHClient()
        ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        ssh.connect('127.0.0.1', 22, username='lixin', password='171901')
        cmd='ls'
        stdin, stdout, stderr = ssh.exec_command(cmd)
        print stdout.readlines()
        ssh.close()

    @QtCore.pyqtSlot()
    def on_pushButton_joystick_clicked(self):
        global joystick
        joystick=1-joystick
        #print joystick
        if joystick:
            self.textBrowser.append('joystick begins taking over!')
            self.textBrowser.moveCursor(QtGui.QTextCursor.End)
        else:
            self.textBrowser.append('joystick stops taking over')
            self.textBrowser.moveCursor(QtGui.QTextCursor.End)
                
        
##       
##    @QtCore.pyqtSlot(str)
##    def on_lineEdit_start_textChanged(self, p0):
##        self.textBrowser.append('test')
##
##    @QtCore.pyqtSlot(str)
##    def on_lineEdit_start2_textChanged(self, p0):
##        self.textBrowser.append('test')
        
##
##    @QtCore.pyqtSlot(str)
##    def on_lineEdit_end_textChanged(self, p0):
##        self.textBrowser.append('test')
##    
##    @QtCore.pyqtSlot(str)
##    def on_lineEdit_end2_textChanged(self, p0):
##        self.textBrowser.append('test')

##def send_CONTROL(ctrl):
##    try:
##        by=struct.pack("BBBBB",0xff,0xaa,0x1,0x1,ctrl)
##        global send_sock
##        #time.sleep(0.1)
##        send_sock.sendall(by)
##    except Exception as err:
##        ui.textBrowser.append("Send control err!")
##        ui.textBrowser.moveCursor(QtGui.QTextCursor.End)
##
##def send_CHANNEL(throttle,roll,pitch,yaw,mode):
##    try:
##        by=struct.pack("BBBBBBBBB",0xff,0xaa,0x2,0x5,pitch/10,roll/10,throttle/10,yaw/10,mode)
##        global send_sock
##        #time.sleep(0.1)
##        send_sock.sendall(by)
##    except Exception as err:
##        ui.textBrowser.append("Send channel err!")
##        ui.textBrowser.moveCursor(QtGui.QTextCursor.End)

def set_slider(throttle,roll,pitch,yaw):
     ui.slider_throto.setValue(throttle)
     ui.label_throto.setText(str(throttle))
     ui.slider_roll.setValue(roll)
     ui.label_roll.setText(str(roll))
     ui.slider_pitch.setValue(pitch)
     ui.label_pitch.setText(str(pitch))
     ui.slider_yaw.setValue(yaw)
     ui.label_yaw.setText(str(yaw))

def set_button(btn1,btn2):
    if (btn1==0 and btn2==0):
        ui.radioButton_stablize.setChecked(True)
        mode=0x1
    else:
        if (btn1==1 and btn2==1):
            ui.radioButton_loiter.setChecked(True)
            mode=0x3
        else:
            if (btn1==1 and btn2==0):
                ui.radioButton_altitude.setChecked(True)
                mode=0x4

  
if __name__ == "__main__":
    def show_ui():
        app =QtGui.QApplication(sys.argv)
        global ui
        ui=main()
        ui.show()
    
        path="/home/lixin/Videos/test.MOV"
        instance=vlc.Instance()
        media=instance.media_new(path)
        player=instance.media_player_new()
        player.set_media(media)
        video_3 = QtGui.QTabWidget(ui.groupBox_2)
##        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Expanding)
##        sizePolicy.setHorizontalStretch(0)
##        sizePolicy.setVerticalStretch(0)
##        sizePolicy.setHeightForWidth(ui.video_3.sizePolicy().hasHeightForWidth())
##        ui.video_3.setSizePolicy(sizePolicy)
        ui.video_3.setMaximumSize(QtCore.QSize(516,367))
        hwnd = int(video_3.winId())
        print hwnd
        player.set_hwnd(hwnd)
        player.play()

        sys.exit(app.exec_())

    def receive_server():
        global receive_sock
        receive_sock=ServerSocket(('10.222.74.34',8008))
        receive_sock.listen(2)
        
        while True:
            c, client = receive_sock.accept()
            ui.textBrowser.append('Receive_server:     Connected!')
            ui.textBrowser.moveCursor(QtGui.QTextCursor.End)
            
            while True:
                try:
                    recv_bytes=receive_sock.recv(4)
                    parity_bit=struct.unpack("BBBB", recv_bytes)
                    #print parity_bit                   
                    if ((parity_bit[0]==0xff)and(parity_bit[1]==0xaa)\
                        and(parity_bit[2]==0xbb)and(parity_bit[3]==0xcc)):  #FFAABBCC
                        #ui.textBrowser.append('Receive_server: get head!')
                        recv_bytes=receive_sock.recv(4)
                        length=struct.unpack("i", recv_bytes)  #length=120
                        #ui.textBrowser.append('Receive_server: get len!')
                        #print length[0]
                        recv_bytes=receive_sock.recv(length[0])
                        
                        #t=struct.unpack("iiiiiiiiiiiiiiiiiiiiiiiiffffff", recv_bytes)
                        #print t
                        arm, xacc, yacc, zacc, motor_speed1, motor_speed2, \
                        motor_speed3, motor_speed4, heading_north, lat, lon, eph, \
                        satellites_visible, chan1, chan2, chan3, chan4, chan5, \
                        chan6, chan7, chan8, vol_remain, cur_remain, bat_remain, \
                        rollspeed,pitchspeed,yawspeed,hud_alt,hud_climb,hud_groundspeed \
                        = struct.unpack("iiiiiiiiiiiiiiiiiiiiiiiiffffff", recv_bytes)                 
                        ui.label_arm.setText(str(arm))
                        ui.label_head_N.setText(str(heading_north))
                        ui.label_satellites.setText(str(satellites_visible))
                        ui.label_GPS_lat.setText(str(lat))
                        ui.label_GPS_lon.setText(str(lon))
                        ui.label_GPS_eph.setText(str(eph))
                        ui.label_X_acc.setText(str(xacc))
                        ui.label_Y_acc.setText(str(yacc))
                        ui.label_Z_acc.setText(str(zacc))
                        ui.label_channel1.setText(str(chan1))
                        ui.label_channel2.setText(str(chan2))
                        ui.label_channel3.setText(str(chan3))
                        ui.label_channel4.setText(str(chan4))
##                        ui.label_channel5.setText(str(chan5))
##                        ui.label_channel6.setText(str(chan6))
##                        ui.label_channel7.setText(str(chan7))
##                        ui.label_channel8.setText(str(chan8))
                        ui.label_motor1.setText(str(motor_speed1))
                        ui.label_motor2.setText(str(motor_speed2))
                        ui.label_motor3.setText(str(motor_speed3))
                        ui.label_motor4.setText(str(motor_speed4))
                        ui.label_vol_remain.setText(str(vol_remain)+'mV')
                        ui.label_cur_remain.setText(str(cur_remain)+'mA')
                        ui.label_bat_remain.setText(str(bat_remain)+'%')
                        s=str(rollspeed)
                        ui.label_rollspeed.setText(s[0:5]+'cm/s')
                        s=str(pitchspeed)
                        ui.label_pitchspeed.setText(s[0:5]+'cm/s')
                        s=str(yawspeed)
                        ui.label_yawspeed.setText(s[0:5]+'cm/s')
                        s=str(hud_alt)
                        ui.label_altitude.setText(s[0:5]+'m')
                        s=str(hud_climb)
                        ui.label_climb_speed.setText(s[0:5]+'cm/s')
                        s=str(hud_groundspeed)
                        ui.label_ground_speed.setText(s[0:5]+'cm/s')
                        
                except Exception as err:
                    ui.textBrowser.append("Receive_server:     Disconnected!")
                    ui.textBrowser.moveCursor(QtGui.QTextCursor.End)
                    break


    def send_server():
        global send_sock
        send_sock=ServerSocket(('10.222.74.34',8000))
        send_sock.listen(2)
        
        while True:
            c, client = send_sock.accept()
            ui.textBrowser.append('Send_server:     Connected!')
            ui.textBrowser.moveCursor(QtGui.QTextCursor.End)
            
            while True:
                try:
                    if ctrl!=0x2:
                        by=struct.pack("BBBBBBBBB",0xff,0xaa,ctrl,0x5,pitch/10,roll/10,throttle/10,yaw/10,mode)
                        time.sleep(5)
                        send_sock.sendall(by)
                        ctrl=0x2
                    by=struct.pack("BBBBBBBBB",0xff,0xaa,ctrl,0x5,pitch/10,roll/10,throttle/10,yaw/10,mode)
                    time.sleep(0.1)
                    send_sock.sendall(by)
                    #print 'sended!'
                except Exception as err:
                    ui.textBrowser.append("Send_server:     Disconnected!!")
                    ui.textBrowser.moveCursor(QtGui.QTextCursor.End)
                    break


    def ctrl_joystick():
        while True:
            try:
                pygame.joystick.init()
                pygame.init()
                _joystick = pygame.joystick.Joystick(0)
                _joystick.init()
                time.sleep(1)
                global joystick_nameoutput
                if joystick_nameoutput==0:
                    ui.textBrowser.append("Get joystick:" + str(_joystick.get_name()))
                    ui.textBrowser.moveCursor(QtGui.QTextCursor.End)
                    joystick_nameoutput=1

                #print joystick
                while joystick:
                    try:
                        time.sleep(0.1)
                        pygame.event.get()
                        throttle=int(_joystick.get_axis(0)*400+1500)         
                        #pygame.event.get()
                        pitch=int(_joystick.get_axis(1)*400+1500)
                        #pygame.event.get()
                        roll=int(_joystick.get_axis(3)*400+1500) 
                        #pygame.event.get()
                        yaw=int(_joystick.get_axis(2)*470+1500)
                        if yaw>1870:
                            yaw=1900
                        elif yaw<1100:
                            yaw=1100
                                 
                        #pygame.event.get()
                        btn1=_joystick.get_button(0)
                        #pygame.event.get()
                        btn2=_joystick.get_button(1)
                        
                        set_button(btn1,btn2)
                        set_slider(throttle,roll,pitch,yaw)
                        
                    except Exception as err:
                        ui.textBrowser.append("Joystick err:     Disconnected!")
                        ui.textBrowser.moveCursor(QtGui.QTextCursor.End)
                        break
                    
            except Exception as err:
                #ui.textBrowser.append("waiting")
                #ui.textBrowser.moveCursor(QtGui.QTextCursor.End) 
                time.sleep(1)
                

                
    ui_thrd=threading.Thread(target=show_ui)
    ui_thrd.start()

    rcv_thrd=threading.Thread(target=receive_server)
    rcv_thrd.daemon=True
    rcv_thrd.start()

    sd_thrd=threading.Thread(target=send_server)
    sd_thrd.daemon=True
    sd_thrd.start()

    time.sleep(3)
    js_thrd=threading.Thread(target=ctrl_joystick)
    js_thrd.daemon=True
    js_thrd.start()


