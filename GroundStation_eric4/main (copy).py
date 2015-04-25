# -*- coding: utf-8 -*-

import sys
import socket
import threading
import paramiko
import struct
import pygame
import time

from PyQt4 import QtCore, QtGui
from Ui_main import Ui_Form
from Server import *
#from qtvlc import Player

global throttle,roll,pitch,yaw,mode,ctrl
throttle=0x44C #1100
roll=0x5DC #1500
pitch=0x5DC #1500
yaw=0x5DC #1500
mode=0x1
ctrl=0x0

send_sock=None

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
        send_CHANNEL(throttle,roll,pitch,yaw,mode)
    
    @QtCore.pyqtSlot(int)
    def on_slider_roll_valueChanged(self, value):
        self.label_roll.setText(str(value))
        global roll
        roll=value
        send_CHANNEL(throttle,roll,pitch,yaw,mode)
    
    @QtCore.pyqtSlot(int)
    def on_slider_pitch_valueChanged(self, value):
        self.label_pitch.setText(str(value))
        global pitch
        pitch=value
        send_CHANNEL(throttle,roll,pitch,yaw,mode)
    
    @QtCore.pyqtSlot(int)
    def on_slider_yaw_valueChanged(self, value):
        self.label_yaw.setText(str(value))
        global yaw
        yaw=value
        send_CHANNEL(throttle,roll,pitch,yaw,mode)
    
    @QtCore.pyqtSlot()
    def on_radioButton_loiter_clicked(self):
        self.textBrowser.append('Mode: loiter')
        global mode
        mode=0x3  #loiter
        send_CHANNEL(throttle,roll,pitch,yaw,mode)
    
    @QtCore.pyqtSlot()
    def on_radioButton_auto_clicked(self):
        self.textBrowser.append('Mode: auto')
        global mode
        mode=0x5  #auto
        send_CHANNEL(throttle,roll,pitch,yaw,mode)
    
    @QtCore.pyqtSlot()
    def on_radioButton_land_clicked(self):
        self.textBrowser.append('Mode: land')
        global mode
        mode=0x2  #land
        send_CHANNEL(throttle,roll,pitch,yaw,mode)
    
    @QtCore.pyqtSlot()
    def on_radioButton_stablize_clicked(self):
        self.textBrowser.append('Mode: stablize')
        global mode
        mode=0x1  #stablize
        send_CHANNEL(throttle,roll,pitch,yaw,mode)
    
    @QtCore.pyqtSlot()
    def on_radioButton_altitude_clicked(self):
        self.textBrowser.append('Mode: altitude')
        global mode
        mode=0x4  #alt_hold
        send_CHANNEL(throttle,roll,pitch,yaw,mode)

    @QtCore.pyqtSlot()
    def on_pushButton_level_clicked(self):
        self.textBrowser.setText('Restart...level')
        global ctrl
        ctrl=0x1  #level
        send_CONTROL(ctrl)

    @QtCore.pyqtSlot()
    def on_pushButton_arm_clicked(self):
        self.textBrowser.append('CTRL: arm')
        global ctrl
        ctrl=0x4  #arm
        send_CONTROL(ctrl)

    @QtCore.pyqtSlot()
    def on_pushButton_disarm_clicked(self):
        self.textBrowser.append('CTRL: disarm')
        global ctrl
        ctrl=0x8  #disarm
        send_CONTROL(ctrl)

    @QtCore.pyqtSlot()
    def on_pushButton_takeoff_clicked(self):
        self.textBrowser.append('CTRL: takeoff')
        global ctrl
        ctrl=0x2  #takeoff
        send_CONTROL(ctrl)

    @QtCore.pyqtSlot()
    def on_pushButton_connect_clicked(self):
        self.textBrowser.append('Begin ssh')
        ssh = paramiko.SSHClient()
        ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        ssh.connect('127.0.0.1', 22, username='lixin', password='171901')
        cmd='ls'
        stdin, stdout, stderr = ssh.exec_command(cmd)
        print stdout.readlines()
        ssh.close()

##    @QtCore.pyqtSlot()
##    def on_pushButton_takeover_clicked(self):
##        try:
##            #print 'ok'
##            pygame.joystick.init()
##            pygame.init()
##            _joystick = pygame.joystick.Joystick(0)
##            _joystick.init()
##            #print "Joystick: get " + str(_joystick.get_name())
##            ui.textBrowser.append("Joystick: get " + str(_joystick.get_name()))
##
##            while pygame.joystick.get_count():
##                #print
##                time.sleep(0.1)
##                pygame.event.get()
##                throttle=int(_joystick.get_axis(0)*400+1500)
##                #print "Axis 0 (throttle) value is:" + str(throttle)           
##                pygame.event.get()
##                pitch=int(_joystick.get_axis(1)*400+1500)
##                #print "Axis 1 (pitch) value is:" + str(pitch)        
##                pygame.event.get()
##                yaw=int(_joystick.get_axis(2)*400+1500)
##                #print "Axis 2 (yaw) value is:" + str(yaw)          
##                pygame.event.get()
##                roll=int(_joystick.get_axis(3)*400+1500)
##                #print "Axis 3 (roll) value is:" + str(roll)   
##                pygame.event.get()
##                #print"Button 0 (mix/gear) value is:" + str(_joystick.get_button(0))
##                pygame.event.get()
##                #print"Button 1 (flap gyro) value is:" + str(_joystick.get_button(1))
##                set_slider(throttle,roll,pitch,yaw)
##                #send_CHANNEL(throttle,roll,pitch,yaw,mode)                
##            pygame.joystick.quit()
##                    
##        except Exception as err:
##                ui.textBrowser.append("Joystick err: Disconnected!")             
                
        
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

def send_CONTROL(ctrl):
    try:
        by=struct.pack("BBBBB",0xff,0xaa,0x1,0x1,ctrl)
        global send_sock
        #time.sleep(0.1)
        send_sock.sendall(by)
    except Exception as err:
        ui.textBrowser.append("Send control err!")

def send_CHANNEL(throttle,roll,pitch,yaw,mode):
    try:
        by=struct.pack("BBBBBBBBB",0xff,0xaa,0x2,0x9,throttle/10,roll/10,pitch/10,yaw/10,mode)
        global send_sock
        #time.sleep(0.1)
        send_sock.sendall(by)
    except Exception as err:
        ui.textBrowser.append("Send channel err!")

def set_slider(throttle,roll,pitch,yaw):
     ui.slider_throto.setValue(throttle)
     ui.label_throto.setText(str(throttle))
     ui.slider_roll.setValue(roll)
     ui.label_roll.setText(str(roll))
     ui.slider_pitch.setValue(pitch)
     ui.label_pitch.setText(str(pitch))
     ui.slider_yaw.setValue(yaw)
     ui.label_yaw.setText(str(yaw))

##def set_button(btn1,btn2):
##    if (btn1==0 and btn2==0):
##        ui.radioButton_altitude.select()
##    else:
##        if (btn1==0 and btn2==1):
##            ui.radioButton_loiter.checked()
##        else:
##            if (btn1==1 and btn2==0):
##                ui.radioButton_stablize.select()

  
if __name__ == "__main__":
    def show_ui():
        app =QtGui.QApplication(sys.argv)
        global ui
        ui=main()
        ui.show()
        sys.exit(app.exec_())

    def receive_server():
        global receive_sock
        receive_sock=ServerSocket(('127.0.0.1',8008))
        
        while True:
            c, client = receive_sock.accept()
            ui.textBrowser.append('Receive_server: Connected!')
            while True:
                try:
                    recv_bytes=receive_sock.recv(4)
                    parity_bit=struct.unpack("BBBB", recv_bytes)
                    #print parity_bit
                    
                    if ((parity_bit[0]==0xff)and(parity_bit[1]==0xaa)\
                        and(parity_bit[2]==0xbb)and(parity_bit[3]==0xcc)): #FFAABBCC
                        ui.textBrowser.append('Receive_server: get head!')
                        recv_bytes=receive_sock.recv(4)
                        length=struct.unpack("i", recv_bytes)
                        ui.textBrowser.append('Receive_server: get len!')#length=120
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
                        ui.label_altitude.setText(s[0:5]+'cm/s')
                        s=str(hud_climb)
                        ui.label_climb_speed.setText(s[0:5]+'cm/s')
                        s=str(hud_groundspeed)
                        ui.label_ground_speed.setText(s[0:5]+'cm/s')


##                        armH,armL,\
##                        xaccH,xaccL,yaccH,yaccL,zaccH,zaccL,\
##                        motor_speed1H,motor_speed1L,motor_speed2H,motor_speed2L,\
##                        motor_speed3H,motor_speed3L,motor_speed4H,motor_speed4L,\
##                        heading_northH,heading_northL,\
##                        latH,latL,lonH,lonL,ephH,ephL,satellites_visibleH,satellites_visibleL,\
##                        chan1H,chan1L,chan2H,chan2L,chan3H,chan3L,chan4H,chan4L,\
##                        chan5H,chan5L,chan6H,chan6L,chan7H,chan7L,chan8H,chan8L,\
##                        vol_remainH,vol_remainL,cur_remainH,cur_remainL,\
##                        bat_remainH,bat_remainL,\
##                        rollspeedH,rollspeedL,pitchspeedH,pitchspeedL,\
##                        yawspeedH,yawspeedL,hud_altH,hud_altL,\
##                        hud_climbH,hud_climbL,hud_groundspeedH,hud_groundspeedL \
##                        = struct.unpack("bBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbB\
##                        bBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbB\
##                        bBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbB", recv_bytes)
##                        
##                        ui.label_arm.setText(str(256*armH+armL))
##                        ui.label_head_N.setText(str(256*heading_northH+heading_northL))
##                        ui.label_satellites.setText(str(256*satellites_visibleH+satellites_visibleL))
##                        ui.label_GPS_lat.setText(str(256*latH+latL))
##                        ui.label_GPS_lon.setText(str(256*lonH+lonL))
##                        ui.label_GPS_eph.setText(str(256*ephH+ephL))
##                        ui.label_X_acc.setText(str(256*xaccH+xaccL))
##                        ui.label_Y_acc.setText(str(256*yaccH+yaccL))
##                        ui.label_Z_acc.setText(str(256*zaccH+zaccL))
##                        ui.label_channel1.setText(str(256*chan1H+chan1L))
##                        ui.label_channel2.setText(str(256*chan2H+chan2L))
##                        ui.label_channel3.setText(str(256*chan3H+chan3L))
##                        ui.label_channel4.setText(str(256*chan4H+chan4L))
####                        ui.label_channel5.setText(str(chan5))
####                        ui.label_channel6.setText(str(chan6))
####                        ui.label_channel7.setText(str(chan7))
####                        ui.label_channel8.setText(str(chan8))
##                        ui.label_motor1.setText(str(256*motor_speed1H+motor_speed1L))
##                        ui.label_motor2.setText(str(256*motor_speed2H+motor_speed2L))
##                        ui.label_motor3.setText(str(256*motor_speed3H+motor_speed3L))
##                        ui.label_motor4.setText(str(256*motor_speed4H+motor_speed4L))
##                        ui.label_vol_remain.setText(str(256*vol_remainH+vol_remainL)+'mV')
##                        ui.label_cur_remain.setText(str(256*cur_remainH+cur_remainL)+'mA')
##                        ui.label_bat_remain.setText(str(256*bat_remainH+bat_remainL)+'%')
##                        ui.label_rollspeed.setText(str(256*rollspeedH+rollspeedL)+'cm/s')
##                        ui.label_pitchspeed.setText(str(256*pitchspeedH+pitchspeedL)+'cm/s')
##                        ui.label_yawspeed.setText(str(256*yawspeedH+yawspeedL)+'cm/s')
##                        ui.label_altitude.setText(str(256*hud_altH+hud_altL)+'cm/s')
##                        ui.label_climb_speed.setText(str(256*hud_climbH+hud_climbL)+'cm/s')
##                        ui.label_ground_speed.setText(str(256*hud_groundspeedH+hud_groundspeedL)+'cm/s')

                except Exception as err:
                    ui.textBrowser.append("Receive_server: Disconnected!")
                    break


    def send_server():
        global send_sock
        send_sock=ServerSocket(('127.0.0.1',8000))
        
        while True:
            c, client = send_sock.accept()
            ui.textBrowser.append('Send_server: Connected!')
            while True:
                try:
                    by=struct.pack("BBBBBBBBB",0xff,0xaa,0x2,0x9,throttle/10,roll/10,pitch/10,yaw/10,mode)
                    time.sleep(0.1)
                    send_sock.sendall(by)
                    #print 'sended!'
                except Exception as err:
                    ui.textBrowser.append("Send channel err!")
                    break


    def ctrl_joystick():
        pygame.joystick.init()
        pygame.init()
        _joystick = pygame.joystick.Joystick(0)
        _joystick.init()
 
        print "Get joystick" + str(_joystick.get_name())
        
        while True:
            try:
                time.sleep(0.1)
                pygame.event.get()
                throttle=int(_joystick.get_axis(0)*400+1500)
                #print "Axis 0 (throttle) value is:" + str(throttle)           
                pygame.event.get()
                pitch=int(_joystick.get_axis(1)*400+1500)
                #print "Axis 1 (pitch) value is:" + str(pitch)        
                pygame.event.get()
                yaw=int(_joystick.get_axis(2)*400+1500)
                #print "Axis 2 (yaw) value is:" + str(yaw)          
                pygame.event.get()
                roll=int(_joystick.get_axis(3)*400+1500)
                #print "Axis 3 (roll) value is:" + str(roll)          
                pygame.event.get()
                #print"Button 0 (mix/gear) value is:" + str(_joystick.get_button(0))
                pygame.event.get()
                #print "Button 1 (flap gyro) value is:" + str(_joystick.get_button(1))
##                btn1=_joystick.get_button(0)
##                btn2=_joystick.get_button(1)
##                set_button(btn1,btn2)
                set_slider(throttle,roll,pitch,yaw)            
            except Exception as err:
                #ui.textBrowser.append("Joystick err: Disconnected!")
                print "Joystick err: Disconnected!"
                break

                
    ui_thrd=threading.Thread(target=show_ui)
    ui_thrd.start()

    rcv_thrd=threading.Thread(target=receive_server)
    rcv_thrd.daemon=True
    rcv_thrd.start()

    sd_thrd=threading.Thread(target=send_server)
    sd_thrd.daemon=True
    sd_thrd.start()

##    js_thrd=threading.Thread(target=ctrl_joystick)
##    js_thrd.start()


