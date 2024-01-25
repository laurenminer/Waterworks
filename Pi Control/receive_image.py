import io
import paramiko
import socket
import struct
from PIL import Image
import cv2
import numpy as np
import time
import threading

# Start a socket listening for connections on 0.0.0.0:8000 (0.0.0.0 means
# all interfaces)

class VideoStream(object):

    def __init__(self,vid_shell,port=8000):
        server_socket = socket.socket()
        server_socket.bind(('0.0.0.0', port))
        server_socket.listen(0)
        self.server_socket = server_socket
        vid_shell.exec_command("python pi_image_stream.py") 
        try:
            threading.Thread(target=lambda: self.set_up_stream(),args=()).start()
        except:
            print "Stream setup error"
        self.frame = None
        self.stopped = False

    def set_up_stream(self):
        # Accept a single connection and make a file-like object out of it
        self.connection = self.server_socket.accept()[0].makefile('rb')
        print "I did it!"

    def play_video(self):
        self.stream_frames()

    def stream_frames(self):
        connection = self.connection
        self.stopped=False
        try:
            while not self.stopped:
                # Read the length of the image as a 32-bit unsigned int. If the
                # length is zero, quit the loop
                image_len = struct.unpack('<L', connection.read(struct.calcsize('<L')))[0]
                if not image_len:
                    pass
                # Construct a stream to hold the image data and read the image
                # data from the connection
                image_stream = io.BytesIO()
                image_stream.write(connection.read(image_len))
                # Rewind the stream, open it as an image with PIL and do some
                # processing on it
                image_stream.seek(0)
                self.frame = Image.open(image_stream).convert('RGB') 
                print "STREAMING"
        except:
            "ERROR IN STREAM FRAMES"
        finally:
            try:
                self.connection.close()
            except:
                pass
            try:
                self.server_socket.close()
            except:
                pass

    def read(self):
        return self.frame

    def stop(self):
        self.stopped = True