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

import heartbeat
shm_id = int(os.environ['VIC_SHM_ID'])
window_size_hr=5
print('		creating hb')
ts=time.time()
hb = heartbeat.Heartbeat(shm_id,window_size_hr,100,"vic.log",10,100)
print('		time took to create hb',time.time()-ts)



import threading
from queue import Queue


# mycam = FoscamCamera('65.114.169.151',88,'admin','admin',daemon=False)
# mycam = FoscamCamera('65.114.169.139',88,'arittenbach','8mmhamcgt16!',daemon=False)
# mycam.ptz_reset()
# vs = VideoStream('rtsp://admin:admin@65.114.169.151:88/videoMain').start() # realvid
# vs = VideoStream('rtsp://arittenbach:8mmhamcgt16!@65.114.169.139:88/videoMain').start() # realvid
vs= FileVideoStream("walkman.mp4").start() # outvid

net = cv2.dnn.readNetFromCaffe("MobileNetSSD_deploy.prototxt.txt", "MobileNetSSD_deploy.caffemodel")
CLASSES = ["background", "aeroplane", "bicycle", "bird", "boat",
	"bottle", "bus", "car", "cat", "chair", "cow", "diningtable",
	"dog", "horse", "motorbike", "person", "pottedplant", "sheep",
	"sofa", "train", "tvmonitor"]
COLORS = np.random.uniform(0, 255, size=(len(CLASSES), 3))

time.sleep(2)

hb.heartbeat_beat()
print('		hb init hr:',hb.get_instant_heartrate())	
xframe = vs.read()

# while vs.more(): # outvid
gpu=(sys.argv[1])
obj=(sys.argv[2])
cnt=0
tn = time.time()


frame=xframe
resized_frame = frame#imutils.resize(frame, width=300)
if 'gpu' in gpu:
	frame = cv2.UMat(resized_frame)
else:
	frame = resized_frame
# if 'gpu' in gpu:
# 	img = cv2.UMat(cv2.imread("image.jpg", cv2.IMREAD_COLOR))
# 	frame = img
# 	# frame = cv2.UMat(img)
# else:
# 	frame = cv2.imread("image.jpg", cv2.IMREAD_COLOR)


while cnt<int(sys.argv[3]): # outvid


	cnt+=1







	if 'obj' in obj:
		meow = vs.read()
		frame=xframe
		resized_frame = imutils.resize(frame, width=300)
		if 'gpu' in gpu:
			frame = cv2.UMat(resized_frame)
		else:
			frame = resized_frame


		(h, w) = resized_frame.shape[:2]
		blob = cv2.dnn.blobFromImage(cv2.resize(frame, (300, 300)),0.007843, (300, 300), 127.5)
		net.setInput(blob)
		detections = net.forward()
		#print(frame.dtype)
		# loop over the detections
		for i in np.arange(0, detections.shape[2]):
			# extract the confidence (i.e., probability) associated with
			# the prediction
			confidence = detections[0, 0, i, 2]
			idx2 = int(detections[0,0,i,1])
			# filter out weak detections by ensuring the `confidence` is
			# greater than the minimum confidence
			if ((confidence > .5) and (CLASSES[idx2]=='person')):
				# extract the index of the class label from the
				# `detections`, then compute the (x, y)-coordinates of
				# the bounding box for the object
				idx = int(detections[0, 0, i, 1])
				box = detections[0, 0, i, 3:7] * np.array([w, h, w, h])
				(startX, startY, endX, endY) = box.astype("int")
			#	print('startX=',startX)
			#	print('endX=',endX)
				label = "{}: {:.2f}%".format(CLASSES[idx],
					confidence * 100)
				cv2.rectangle(frame, (startX, startY), (endX, endY),
					COLORS[idx], 2)
				y = startY - 15 if startY - 15 > 15 else startY + 15
				cv2.putText(frame, label, (startX, y),
					cv2.FONT_HERSHEY_SIMPLEX, 0.5, COLORS[idx], 2)
		cv2.imshow("Frame", frame)

	else:

		# if 'gpu' in gpu:
		# 	img = cv2.UMat(cv2.imread("image.jpg", cv2.IMREAD_COLOR))
		# 	frame = cv2.UMat(img)
		# else:
		# 	frame = cv2.imread("image.jpg", cv2.IMREAD_COLOR)

		gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
		gray = cv2.GaussianBlur(gray, (7, 7), 1.5)
		gray = cv2.Canny(gray, 0, 50)
		cv2.imshow("Frame", gray)


	# gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
	# gray = cv2.GaussianBlur(gray, (7, 7), 1.5)
	# gray = cv2.Canny(gray, 0, 50)
	# cv2.imshow("Frame", gray)




	key = cv2.waitKey(1) & 0xFF
	# if the `q` key was pressed, break from the loop
	if key == ord("q"):
		break
	hb.heartbeat_beat()
	print('		hb: gpu exec time:',1/hb.get_instant_heartrate())
	print(time.time()-tn,'exec time')
	tn=time.time()
cv2.destroyAllWindows()
vs.stop()
hb.heartbeat_finish()


