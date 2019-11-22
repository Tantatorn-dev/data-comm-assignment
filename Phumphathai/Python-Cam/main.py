import serial
import time
import os
import numpy as np
import cv2


class Camera:
    def __init__(self, port='COM3', height=160, width=120):
        self.COMMAND = ['*', 'R', 'D', 'Y', '*']

        self.height = height
        self.width = width

        self.serial = serial.Serial()
        self.serial.port = port
        self.serial.baudrate = 1000000
        self.serial.bytesize = serial.EIGHTBITS
        self.serial.stopbits = serial.STOPBITS_ONE
        self.serial.parity = serial.PARITY_NONE
        self.serial.timeout = 0.2

    def connect(self):
        self.serial.open()
        while not self.isImageStart():
            pass
        print("finish init camera")

    def read(self):
        raw = self.serial.read()
        if (len(raw) != 1):
            return False
        return int.from_bytes(raw, "big") & 0xFF

    def isImageStart(self, index=0):
        if index < len(self.COMMAND):
            a = self.read()
            if a == False or a >= 128:
                return False
            a = chr(a)
            if self.COMMAND[index] == a:
                return self.isImageStart(index + 1)
            else:
                return False
        return True

    def getImg(self):
        self.serial.write([ord('A')])

        while not self.isImageStart():
            pass

        img_arr = np.zeros([self.height, self.width, 1], dtype=np.uint8)

        for x in range(img_arr.shape[1]):
            for y in range(img_arr.shape[0]):
                temp = self.read()
                if temp == False:
                    return False
                img_arr[y, self.width - x - 1] = [temp]

        cv2.destroyAllWindows()
        cv2.imshow('a', img_arr)
        cv2.waitKey(10)

        return img_arr


cam = Camera('COM3')
time.sleep(2)
cam.connect()
while True:
    cam.getImg()
