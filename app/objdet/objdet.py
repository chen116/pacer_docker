# import the necessary packages
# https://www.learnopencv.com/opencv-transparent-api/
from imutils.video import VideoStream
from imutils.video import FPS
from foscam_v3 import FoscamCamera
import numpy as np

import imutils
import time
import cv2
import os
import socket
import sys
import threading
import copy
from imutils.video import FileVideoStream



import threading
from queue import Queue



# mycam = FoscamCamera('65.114.169.151',88,'admin','admin',daemon=False)
# mycam = FoscamCamera('65.114.169.139',88,'arittenbach','8mmhamcgt16!',daemon=False)
# mycam.ptz_reset()
# vs = VideoStream('rtsp://admin:admin@65.114.169.151:88/videoMain').start() # realvid
# vs = VideoStream('rtsp://arittenbach:8mmhamcgt16!@65.114.169.139:88/videoMain').start() # realvid
vs= FileVideoStream("walkman.mp4").start() # outvid


time.sleep(2)

while vs.more(): # outvid


	frame = vs.read()
	# img = cv2.UMat(cv2.imread("image.jpg", cv2.IMREAD_COLOR))
	frame = cv2.UMat(frame)

	# frame = imutils.resize(frame, width=400)
	cv2.imshow("Frame", frame)
	key = cv2.waitKey(1) & 0xFF
	# if the `q` key was pressed, break from the loop
	if key == ord("q"):
		break
cv2.destroyAllWindows()
vs.stop()


