import time
import serial
import sys
import glob
ardLoc = glob.glob('/dev/ttyACM*')[0]
ser = serial.Serial(ardLoc,9600,timeout=1)
time.sleep(1)
ser.write('blocks')
while True:
	inNum = bytes(raw_input("How would you like to stimulate? Syntax:\n well number, frequency (Hz), pulse width (ms), block duration (ms), block period (ms) \n e.g. 5, 10, 20, 1000, 5000 means 10 Hz 20 ms stimulus in well #5 for 1 second with 5 seconds in between the start of each block of flashes \n"))
	ser.write(inNum)
	print time.strftime("%H:%M:%S",time.localtime())
