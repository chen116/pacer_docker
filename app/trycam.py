# import the necessary packages
from imutils.video import VideoStream
from imutils.video import FPS
from foscam_v3 import FoscamCamera
import time
import cv2
import imutils




mycam = FoscamCamera('65.114.169.151',88,'admin','admin',daemon=False)
mycam.ptz_reset()
vs = VideoStream('rtsp://admin:admin@65.114.169.151:88/videoMain').start() # realvid
time.sleep(2)
while True: # realvid

	frame = vs.read()
	frame = imutils.resize(frame, width=400)
	cv2.imshow("Frame", frame)
	key = cv2.waitKey(1) & 0xFF
	# if the `q` key was pressed, break from the loop
	if key == ord("q"):
		break
cv2.destroyAllWindows()
vs.stop()


