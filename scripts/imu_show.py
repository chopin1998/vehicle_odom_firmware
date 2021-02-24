#!/usr/bin/python3

import sys
import math
import numpy as np

from PyQt5.QtCore import pyqtSignal, QPoint, QSize, Qt, QTimer
from PyQt5.QtGui import QColor, QOpenGLVersionProfile
from PyQt5.QtWidgets import (QApplication, QHBoxLayout, QOpenGLWidget, QSlider,
                             QWidget)

import serial
import struct


class Window(QWidget):
    def __init__(self):
        super(Window, self).__init__()

        self.glWidget = GLWidget()

        mainLayout = QHBoxLayout()
        mainLayout.addWidget(self.glWidget)
        self.setLayout(mainLayout)

        self.setWindowTitle("IMU 9DoF show")

        self.init_dev()
        self.init_timer()

    def init_dev(self):
        DEV_NAME = '/dev/serial/by-id/usb-tensor-robotics_OF_IMU_3159365B3438-if00'
        try:
            self.ser = serial.Serial(DEV_NAME, 2000000, timeout=0.1)
            self.ser.flush()
        except Exception as ex:
            print('%s' % ex)
            sys.exit(-1)

    def init_timer(self):
        self.timer = QTimer(self)
        self.timer.timeout.connect(self.timer_slot)
        self.timer.start(5)

    def timer_slot(self):

        data = self.ser.readline()

        # print(data)
        data = data.split(b',')
        x = float(data[7]) / 100
        y = float(data[8]) / 100
        z = float(data[9]) / 10

        self.glWidget.setXRotation(x)
        self.glWidget.setYRotation(y)
        self.glWidget.setZRotation(z)
        
        print(data)



class GLWidget(QOpenGLWidget):
    xRotationChanged = pyqtSignal(float)
    yRotationChanged = pyqtSignal(float)
    zRotationChanged = pyqtSignal(float)

    def __init__(self, parent=None):
        super(GLWidget, self).__init__(parent)

        self.object = 0
        self.xRot = 0
        self.yRot = 0
        self.zRot = 0

        self.lastPos = QPoint()

        self.front_color = QColor.fromCmykF(0.1, 0.0, 1.0, 0.0)
        self.backg_color = QColor.fromCmykF(0.0, 0.0, 0.0, 0.0)

    def minimumSizeHint(self):
        return QSize(50, 50)

    def sizeHint(self):
        return QSize(400, 400)

    def setXRotation(self, angle):
        # angle = self.normalizeAngle(angle)
        if angle != self.xRot:
            self.xRot = angle
            self.xRotationChanged.emit(angle)
            self.update()

    def setYRotation(self, angle):
        # angle = self.normalizeAngle(angle)
        if angle != self.yRot:
            self.yRot = angle
            self.yRotationChanged.emit(angle)
            self.update()

    def setZRotation(self, angle):
        # angle = self.normalizeAngle(angle)
        if angle != self.zRot:
            self.zRot = angle
            self.zRotationChanged.emit(angle)
            self.update()

    def initializeGL(self):
        version_profile = QOpenGLVersionProfile()
        version_profile.setVersion(2, 0)
        self.gl = self.context().versionFunctions(version_profile)
        self.gl.initializeOpenGLFunctions()

        self.setClearColor(self.backg_color.darker())
        self.object = self.makeObject()
        self.gl.glShadeModel(self.gl.GL_FLAT)
        self.gl.glEnable(self.gl.GL_DEPTH_TEST)
        self.gl.glEnable(self.gl.GL_CULL_FACE)

    def paintGL(self):
        self.gl.glClear(
            self.gl.GL_COLOR_BUFFER_BIT | self.gl.GL_DEPTH_BUFFER_BIT)
        self.gl.glLoadIdentity()
        self.gl.glTranslated(0.0, 0.0, -10.0)
        self.gl.glRotated(self.xRot, 1.0, 0.0, 0.0)
        self.gl.glRotated(self.yRot, 0.0, 1.0, 0.0)
        self.gl.glRotated(self.zRot, 0.0, 0.0, 1.0)
        self.gl.glCallList(self.object)

    def resizeGL(self, width, height):
        side = min(width, height)
        if side < 0:
            return

        self.gl.glViewport((width - side) // 2, (height - side) // 2, side,
                           side)

        self.gl.glMatrixMode(self.gl.GL_PROJECTION)
        self.gl.glLoadIdentity()
        self.gl.glOrtho(-0.5, +0.5, +0.5, -0.5, 4.0, 15.0)
        self.gl.glMatrixMode(self.gl.GL_MODELVIEW)

    def makeObject_empty(self):
        gen_list = self.gl.glGenLists(1)
        self.gl.glNewList(gen_list, self.gl.GL_COMPILE)

        self.gl.glBegin(self.gl.GL_QUADS)

        self.gl.glEnd()
        self.gl.glEndList()

        return gen_list

    def makeObject(self):
        genList = self.gl.glGenLists(1)
        self.gl.glNewList(genList, self.gl.GL_COMPILE)

        self.gl.glBegin(self.gl.GL_QUADS)

        x1 = +0.0  # +0.06
        y1 = -0.0  # -0.14
        x2 = +0.0  # +0.14
        y2 = -0.0  # -0.06
        x3 = +0.08
        y3 = +0.00
        x4 = +0.30
        y4 = +0.22

        self.quad(x1, y1, x2, y2, y2, x2, y1, x1)
        self.quad(x3, y3, x4, y4, y4, x4, y3, x3)

        self.extrude(x1, y1, x2, y2)
        self.extrude(x2, y2, y2, x2)
        self.extrude(y2, x2, y1, x1)
        self.extrude(y1, x1, x1, y1)
        self.extrude(x3, y3, x4, y4)
        self.extrude(x4, y4, y4, x4)
        self.extrude(y4, x4, y3, x3)

        NumSectors = 200

        for i in range(NumSectors):
            angle1 = (i * 2 * math.pi) / NumSectors
            x5 = 0.30 * math.sin(angle1)
            y5 = 0.30 * math.cos(angle1)
            x6 = 0.20 * math.sin(angle1)
            y6 = 0.20 * math.cos(angle1)

            angle2 = ((i + 1) * 2 * math.pi) / NumSectors
            x7 = 0.20 * math.sin(angle2)
            y7 = 0.20 * math.cos(angle2)
            x8 = 0.30 * math.sin(angle2)
            y8 = 0.30 * math.cos(angle2)

            self.quad(x5, y5, x6, y6, x7, y7, x8, y8)

            self.extrude(x6, y6, x7, y7)
            self.extrude(x8, y8, x5, y5)

        self.gl.glEnd()
        self.gl.glEndList()

        return genList

    def quad(self, x1, y1, x2, y2, x3, y3, x4, y4):
        self.setColor(self.front_color)

        self.gl.glVertex3d(x1, y1, -0.05)
        self.gl.glVertex3d(x2, y2, -0.05)
        self.gl.glVertex3d(x3, y3, -0.05)
        self.gl.glVertex3d(x4, y4, -0.05)

        self.gl.glVertex3d(x4, y4, +0.05)
        self.gl.glVertex3d(x3, y3, +0.05)
        self.gl.glVertex3d(x2, y2, +0.05)
        self.gl.glVertex3d(x1, y1, +0.05)

    def extrude(self, x1, y1, x2, y2):
        self.setColor(self.front_color.darker(250 + int(100 * x1)))

        self.gl.glVertex3d(x1, y1, +0.05)
        self.gl.glVertex3d(x2, y2, +0.05)
        self.gl.glVertex3d(x2, y2, -0.05)
        self.gl.glVertex3d(x1, y1, -0.05)

    def setClearColor(self, c):
        self.gl.glClearColor(c.redF(), c.greenF(), c.blueF(), c.alphaF())

    def setColor(self, c):
        self.gl.glColor4f(c.redF(), c.greenF(), c.blueF(), c.alphaF())


if __name__ == '__main__':

    app = QApplication(sys.argv)
    window = Window()
    window.show()
    sys.exit(app.exec_())
