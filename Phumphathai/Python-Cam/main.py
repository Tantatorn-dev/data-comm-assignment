import time
import os

from camera import Camera

cam = Camera('COM3')
cam.connect()
time.sleep(2)
i = 0
while True:
    img, Err = cam.getImg()
    cam.display(img)
    if not Err:
        cam.save(img, 'out\\' + str(i) + '.jpg')
        i = i+1
        # time.sleep(5)
