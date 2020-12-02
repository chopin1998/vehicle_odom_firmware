#!/usr/bin/python3

import math
import numpy as np
import time
import serial

dev = serial.Serial(
    '/dev/serial/by-id/usb-tensor-robotics_OpticalFlow_IMU_3159365B3438-if00', 2000000, timeout=0.1)


def q2e(w, x, y, z):
    ysqr = y * y

    t0 = +2.0 * (w * x + y * z)
    t1 = +1.0 - 2.0 * (x * x + ysqr)
    X = np.degrees(np.arctan2(t0, t1))

    t2 = +2.0 * (w * y - z * x)
    t2 = np.where(t2 > +1.0, +1.0, t2)
    #t2 = +1.0 if t2 > +1.0 else t2

    t2 = np.where(t2 < -1.0, -1.0, t2)
    #t2 = -1.0 if t2 < -1.0 else t2
    Y = np.degrees(np.arcsin(t2))

    t3 = +2.0 * (w * z + x * y)
    t4 = +1.0 - 2.0 * (ysqr + z * z)
    Z = np.degrees(np.arctan2(t3, t4))

    return X, Y, Z


while 1:
    data = dev.readline()[:-1].split(b' ')

    w, x, y, z = map(float, data)

    # print([w, x, y, z])
    x,y,z = q2e(w, x, y, z)

    print('%4.2f\t%4.2f\t%4.2f' %(x, y, z))
