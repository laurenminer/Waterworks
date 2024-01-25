import time
import serial
import sys
ser = serial.Serial('/dev/ttyACM0',9600,timeout=1)
time.sleep(1)
ser.write('zhang')
while True:
	inNum = bytes(raw_input("How would you like to stimulate? Syntax:\n delay (min), pulse width (ms) \n e.g. 10, 5000 means wait 10 minutes, then turn on the light for 5 seconds. \n"))
	ser.write(inNum)
	print time.strftime("%H:%M:%S",time.localtime())
