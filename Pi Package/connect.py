## Updated to use sockets instead of paramiko. Better in some ways, worse in others. Anyhow, at least I learned a little about sockets.
##
## SCT 01/23/2018

import time
import serial
import sys
import glob
import threading
import socket
import StringIO
from logExperiment import Experiment

def connect_to_the_client(client_ready):
	# Quite sure this is bad code
	# Connect to the client computer controlling everyone
	server_socket = socket.socket()
	server_socket.bind(('',8400))
	server_socket.listen(0)
	print("listening")
	connection, address = server_socket.accept()
	client_ready.set()
	print("connected")
	# This socket determines whether or not to shut everything down
	try:
		# Now wait for other info from the pi
		keep_going = True
		while keep_going:
			data = connection.recv(4096)
			if not (data == "On"):
				print("Exiting")
				exit_out()
				keep_going = False
	except:
		print("Error in stay_connected (connect_to_the_client)")
	finally:
		#exit_out()
		connection.close()
		server_socket.close()

def run_protocol(protocol_holder, protocol_acquired):
	# get the protocol / experiment going
	protocol_socket = socket.socket()
	protocol_socket.bind(('',8600))
	protocol_socket.listen(0)
	print("waiting for protocol")
	prot_con, prot_add = protocol_socket.accept()
	print("connected for protocol")
	protocol = prot_con.recv(4096)
	print("protocol is %s" %protocol)
	protocol_holder[0] = protocol
	protocol_acquired.set()
	prot_con.close()
	protocol_socket.close()
	
	# communicate with the arduino using this socket, new lines will send via the serial port
	ard_socket = socket.socket()
	ard_socket.bind(('',8900))
	ard_socket.listen(0)
	print("waiting for arduino command")
	ard_con, ard_add = ard_socket.accept()
	print("connected for arduino control")


def arduino_connection(protocol):
	# connect to the arduino
	global connect_to_arduino
	connect_to_arduino = True
	command_dict={"Paired pulse":"pairedPulse","Flashing Lights":"singleWells","Blocks":"blocks"}
	ardLoc = glob.glob('/dev/ttyACM*')[0]
	ser = serial.Serial(ardLoc,9600,timeout=1)
	time.sleep(1)
	ser.write(command_dict[protocol_holder[0]])
	buff = StringIO.StringIO(4096) # Some decent size, to avoid mid-run expansion
	expt = Experiment()
	expt.note_change("Initiated experiment: "+protocol_holder[0])
	while connect_to_arduino:
		data = ard_con.recv(4096)                  # Pull what it can
		buff.write(data)                    # Append that segment to the buffer
		if '\n' in data:             # If that segment had '\n', break
			ser.write(buff.getvalue())
			print(buff)
			print(buff.getvalue())
			print(time.strftime("%H:%M:%S",time.localtime()))
			expt.note_change('Entered command: '+str(buff.getvalue()))
			buff = StringIO.StringIO(4096)
	print("End of arduino connection")
	

def exit_out():
	# close up shop
	global connect_to_arduino
	connect_to_arduino = False
	#connection.close()
	#server_socket.close()
	ard_con.close()
	ard_socket.close()

# connect to the client
client_ready = threading.Event()
connect_thread = threading.Thread(target=connect_to_the_client, args=(client_ready,))
connect_thread.start()
client_ready.wait()

protocol_holder = []
protocol_acquired = threading.Event()
protocol_thread = threading.Thread(target=run_protocol, args=(protocol_holder, protocol_acquired))
protocol_thread.start()
protocol_acquired.wait()

# once you've learned the protocol, get the arduino running
connect_to_arduino = True
arduino_thread = threading.Thread(target=arduino_connection)
arduino_thread.start()
