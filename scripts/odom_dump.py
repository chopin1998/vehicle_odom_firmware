#!/usr/bin/python3

import sys
import struct
import serial

class OdomDump( object ):

    DEV_NAME = '/dev/serial/by-id/usb-tensor-robotics_IMU_OPTICALFLOW_3159365B3438-if00'

    def __init__(self) -> None:
        
        try:
            self.dev = serial.Serial(self.DEV_NAME, 2000000, timeout=0.05)
            self.dev.write(b'mode1')
        except Exception as ex:
            print('failed to open dev')
            sys.exit(-1)
    
    def loop(self) -> None:

        try:
            self.dev.flush()
        except Exception as ex:
            print('flush dev error')
            sys.exit(-1)
        
        print('\033[2J') # clear screen
        print('\033[?25l') # hide cursor
        while 1:

            try:
                data = self.dev.readline().strip()
            except Exception as ex:
                print('read dev error')
                sys.exit(-1)

            try:
                imu_data, odom_data = data.split(b'|')
            except Exception as ex:
                print(ex, data)
                continue
            
            try:
                imu_data = list(map(int, imu_data.split(b',')))
            except Exception as ex:
                print(ex, imu_data)
                continue
            imu_data[0] /= 100
            imu_data[1] /= 100
            imu_data[2] /= 10

            odom_data = odom_data.split(b',')
            odom_data[0] = int(odom_data[0], 16)
            odom_data[1:] = list(map(int, odom_data[1:]))

            # print(imu_data, odom_data, end='\r')
            for i in imu_data:
                if i < 0:
                    print(i, ' '*4)
                else:
                    print('', i, ' '*4)
            
            print('--'*4)

            for i in odom_data:
                if i < 0:
                    print(i, ' '*4)
                else:
                    print('', i, ' '*4)
            
            print('\033[%dA' %(12)) # move cursor N line up




########################
if __name__ == "__main__":
    odom = OdomDump()
    odom.loop()


