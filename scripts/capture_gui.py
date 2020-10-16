#!/usr/bin/python


import sys
import time
import serial

from PyQt5 import QtCore, QtGui, QtWidgets
from PIL import Image, ImageQt


class ADNS3080( object ):

    def __init__(self, dev='/dev/ttyACM0', baud=1000000):

        self.dev = serial.Serial(dev, baud, timeout=0.01)
        self.dev.flush()

    def capture(self):

        self.dev.flush()
        self.dev.write(b'cap')

        try:
            rev = self.dev.read(900)
        except Exception as ex:
            return (False, ex)

        return (True, rev)

    def motion(self):

        self.dev.flush()
        self.dev.write('mon')

        try:
            rev = self.dev.readline()
        except Exception as ex:
            return (False, ex)

        return (True, rev.strip())


class CapGui( QtWidgets.QWidget ):

    def __init__(self):
        self.adns3080 = ADNS3080()
        
        super(CapGui, self).__init__()

        self.img_label = QtWidgets.QLabel(self)
        self.img_label.resize(600, 600)

        self.btn = QtWidgets.QPushButton(self)
        self.btn.clicked.connect(self.click)

        self._last_frame_time = time.time()
        
        timer = QtCore.QTimer(self)
        timer.timeout.connect(self.show_img)
        timer.start(33)

    def click(self):
        print('click')
        self.show_img()

    def show_img(self):

        if len(sys.argv) == 1:
            data = self.adns3080.capture()
            if data[0]:
                t = time.time()
                print(t - self._last_frame_time)
                self._last_frame_time = t
                
                img = Image.new('L', (30, 30))
                img.frombytes(data[1])
                img = img.resize((600, 600))
                img = ImageQt.ImageQt(img)
                
                self.img_label.setPixmap(QtGui.QPixmap.fromImage(img))
                #self.img_label.setPixmap(QtGui.QPixmap.fromString(img.tobytes()))
                self.img_label.show()
            else:
                print('failed to capture frame')
                
        else:
            data = self.adns3080.motion()
            if data[0]:
                print(data[1])
            else:
                print('failed to get motion info')
    

#################################3

if __name__ == '__main__':

    #s = ImageSource()
    #print s.capture()
    app = QtWidgets.QApplication(sys.argv)

    w = CapGui()
    w.show()

    app.exec_()
