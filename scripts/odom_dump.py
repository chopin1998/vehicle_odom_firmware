#!/usr/bin/python3

import sys
import struct
import serial
import signal
import atexit

class OdomDump( object ):

    DEV_NAME = '/dev/serial/by-id/usb-tensor-robotics_OF_IMU_3159365B3438-if00'

    def __init__(self) -> None:
        
        atexit.register(self._exit)
        signal.signal(signal.SIGINT, sys.exit)

        try:
            self.dev = serial.Serial(self.DEV_NAME, 2000000, timeout=0.05)
            self.dev.write(b'mode1')
        except Exception as ex:
            print('failed to open dev')
            sys.exit(-1)
    
    def _exit(self):
        print('\033[?25h')
        print('bye..')

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
                data = data.split(b',')
                if len(data) != 24:
                    # print(data)
                    continue
                tick_delta, data = data[0], data[1:]
                imu_data, odom_data = data[:9], data[9:]
            except Exception as ex:
                print(ex, data)
                continue
            
            try:
                imu_data = list(map(int, imu_data))
            except Exception as ex:
                print(ex, imu_data)
                continue

            imu_data[3] /= 10
            imu_data[4] /= 10
            imu_data[5] /= 10

            imu_data[6] /= 100
            imu_data[7] /= 100
            imu_data[8] /= 10

            odom0_data, odom1_data = odom_data[:7], odom_data[7:]

            odom0_data[0] = int(odom0_data[0], 16)
            odom0_data[1:] = list(map(int, odom0_data[1:]))

            odom1_data[0] = int(odom1_data[0], 16)
            odom1_data[1:] = list(map(int, odom1_data[1:]))

            # print(imu_data, odom_data, end='\r')
            print('tick delta: %s' %int(tick_delta))
            print('=='*4)
            for i in imu_data:
                if i < 0:
                    print(i, ' '*4)
                else:
                    print('', i, ' '*4)
            
            print('--'*4)

            for i in odom0_data:
                if i < 0:
                    print(i, ' '*4)
                else:
                    print('', i, ' '*4)

            print('--'*4)

            for i in odom1_data:
                if i < 0:
                    print(i, ' '*4)
                else:
                    print('', i, ' '*4)
            
            print('\033[%dA' %(28)) # move cursor N line up




########################
if __name__ == "__main__":
    odom = OdomDump()
    odom.loop()


