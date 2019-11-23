import time
import os

from camera import Camera
from image import getImageData


cam = Camera('COM3')
cam.connect()
time.sleep(2)

i = 0
while True:
    img, Err = cam.getImg()
    cam.display(img)
    if not Err:
        typeByte = getImageData(img)
