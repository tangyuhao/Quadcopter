import sys
from PyQt4 import Qt
import vlc

app = Qt.QApplication(sys.argv)
video = Qt.QWidget()
video.setMaximumSize(300, 300)
path="/home/lixin/Videos/test.MOV"
instance=vlc.Instance()
media=instance.media_new(path)
player=instance.media_player_new()
player.set_media(media)

hwnd = int(video.winId())
print hwnd
player.set_hwnd('-wid',hwnd)

video.show()
player.play()

app.exec_()
