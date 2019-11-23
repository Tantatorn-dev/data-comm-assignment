import time
import os

import serial

from camera import Camera
from image import getImageData

ser = serial.Serial()
ser.port = 'COM15'
ser.baudrate = 115200
ser.bytesize = serial.EIGHTBITS
ser.stopbits = serial.STOPBITS_ONE
ser.parity = serial.PARITY_NONE
ser.timeout = 0.4
ser.open()

cam = Camera('COM3')
cam.connect()
time.sleep(2)

i = 0
while True:
    while not ser.inWaiting():
        time.sleep(0.2)
    a = ser.read_until()
    print("Ser", a.decode('ascii'))
    Err = True
    while Err:
        img, Err = cam.getImg()
        cam.display(img)
        if not Err:
            typeByte = getImageData(img)
            if typeByte == '0':
                Err = True
                continue
            ser.write(typeByte.encode('ascii'))
            cam.save(img, 'out\\' + str(i) + '.png')
            i = i+1
