import io
import socket
import struct
import time
import picamera
import cv2
from picamera.array import PiRGBArray
# connect to server
client_socket = socket.socket()
client_socket.connect(('10.23.48.240',8550))

# make file-like object
connection = client_socket.makefile('wb')
try:
	with picamera.PiCamera() as camera:
		camera.resolution = (1920,1080)
		#camera.resolution = (1280,720)
		# start preview, warm up
		camera.framerate = 20
		rawCapture = PiRGBArray(camera)
		time.sleep(0.5)

		# note start time, construct stream to hold data
		start = time.time()
		stream = io.BytesIO()
		for frame in camera.capture_continuous(stream,format='jpeg'):
			# write length of stream, then flush to send
			connection.write(struct.pack('<L',stream.tell()))
			connection.flush()
			# rewind straem, send image data
			stream.seek(0)
			connection.write(stream.read())
			# how long before terminating
			#if time.time() - start > 100:
			#	break
			# reset stream for next capture
			stream.seek(0)
			stream.truncate()
	# say you're done by writing a length of 0
	connection.write(struct.pack('<L',0))
finally:
	connection.close()
	client_socket.close()
