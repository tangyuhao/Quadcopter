#!/usr/bin/env python
# -*- coding: utf-8 -*-
import mpylayer
from PyQt4 import QtGui, QtCore

class VidPlayer(QtGui.QMainWindow):
	def __init__(self, parent=None):
		super(VidPlayer, self).__init__(parent)
		self.centralWidget = QtGui.QWidget(self)
		self.video = QtGui.QTabWidget(self.centralWidget)
		self.video.setStyleSheet('background: white')
		self.playButton = QtGui.QPushButton('Play', self.centralWidget)
		self.stopButton = QtGui.QPushButton('Stop', self.centralWidget)
		self.buttonsLayout = QtGui.QHBoxLayout()
		self.buttonsLayout.addWidget(self.playButton)
		self.buttonsLayout.addWidget(self.stopButton)
		self.mainLayout = QtGui.QVBoxLayout(self.centralWidget)
		self.mainLayout.addWidget(self.video)
		self.mainLayout.addLayout(self.buttonsLayout)
		self.setCentralWidget(self.centralWidget)
		
		self.mplayer = mpylayer.MPlayerControl(extra_args=['-wid', str(self.video.winId())])
		
		self.connect(self.playButton, QtCore.SIGNAL('clicked(bool)'), self.play)
		self.connect(self.stopButton, QtCore.SIGNAL('clicked(bool)'), self.stop)
		
	def play(self):
		self.mplayer.loadfile('/home/lixin/Videos/test.MOV')
		
	def stop(self):
		self.mplayer.stop()
		

if __name__ == '__main__':
	import sys
	app = QtGui.QApplication(sys.argv)
	vp = VidPlayer()
	vp.show()
	sys.exit(app.exec_())
##--- mpylayer-0.2a1/mpylayer/mpylayer_control.py	2010-02-13 01:51:58.0 -0600
##+++ vidtracker/mpylayer/mpylayer_control.py	2010-02-14 01:00:54.0 -0600
##@@ -204,23 +204,32 @@
## def _read_all(self):
## while True:
## rlst, wlst, elst = select([self._mp.stdout], [], [], 0)
## if not rlst:
## break
##+if len(self._buffer) == 0: self._buffer.append('')
## for r in rlst:
##-self._buffer.append(r.readline())
##+byte = r.read(1);
##+self._buffer[-1] += byte
##+if byte == '\n':
##+self._buffer.append('')
## if self._buffer:
## _debug(self._buffer)
## 
## def _flush(self):
## self._read_all()
## self._buffer.clear()
## 
## def _get_result(self):
## self._read_all()
## if self._buffer:
##-value = self._buffer.pop()
##+value = ''
##+while value == '':
##+try:
##+value = self._buffer.pop()
##+except IndexError:
##+value = ''
## if '=' in value:
## foo, foo, value = value.partition('=')
## return value.strip(').strip()
## else:
## return ''
