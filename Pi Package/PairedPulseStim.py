import time
import serial
import sys
import glob
ardLoc = glob.glob('/dev/ttyACM*')[0]
ser = serial.Serial(ardLoc,9600,timeout=1)
time.sleep(1)
ser.write('pairedPulse')
while True:
	inNum = bytes(raw_input("How would you like to stimulate? Syntax:\n well number, delay (min), pulse width (ms), rest duration (s), second pulse (ms) \n e.g. 5, 10, 5000, 30, 5000 means wait 10 minutes, then turn on well #5 for 5 seconds. Then wait 30 seconds, after which turn on the light again for 5 seconds\n"))
	ser.write(inNum)
	print time.strftime("%H:%M:%S",time.localtime())
