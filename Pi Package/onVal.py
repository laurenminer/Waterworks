import time
import serial
import sys
ser = serial.Serial('/dev/ttyACM0',9600,timeout=1)
time.sleep(1)
ser.write('onval')
inNum = bytes(raw_input("Input light intensity as fraction of maximum \n e.g. 0.5 for 50%, 0.3 for 30%, 1.0 for 100%\n"))
ser.write(inNum)
