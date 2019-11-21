import serial
import time
import os
import numpy as np
import cv2
from matplotlib import pyplot as plt
from gen_image import MyImage

def getLatestFile(path):
    files = os.listdir(path)
    return files[-1]

def getLatesImgType():
    # get latest image
    path = 'C:\\out'
    fileName = getLatestFile(path)
    image = cv2.imread(path + "\\" + fileName)

    # reduce noise and convert to gray scale
    blur = cv2.GaussianBlur(image,(5,5),0)
    rotated = cv2.rotate(blur, cv2.ROTATE_180)
    gray = cv2.cvtColor(rotated, cv2.COLOR_BGR2GRAY)

    # mapping useful for low brightness image
    def mapImg(img):
        scale = np.amax(img) - np.amin(img)
        rows = img.shape[0]
        cols = img.shape[1]
        newImg = np.zeros((rows, cols), np.uint8)
        for r in range(rows):
            for c in range(cols):
                newVal = np.uint8(((img[r][c]-np.amin(img)) / scale) * 255)
                newImg[r][c] = newVal
        return newImg
    n = 4
    mapped = mapImg(cv2.resize(gray, (gray.shape[1] // n,gray.shape[0] // n)))

    # first threshold for mapped image
    retVal, thresh = cv2.threshold(mapped, 80, 255, cv2.THRESH_BINARY) 

    # second threshold for 24 x 24 image
    x = 24
    dim = (x, x)
    resize24 = cv2.resize(thresh, dim)
    retValV2, thresh24 = cv2.threshold(resize24, 150, 255, cv2.THRESH_BINARY)

    # final threshold for 4 x 4 image
    dim4 = (4,4)
    resize4 = cv2.resize(thresh24, dim4)
    ret, thresh4 = cv2.threshold(resize4, 150, 255, cv2.THRESH_BINARY)

    img24 = thresh24
    img4 = thresh4

    return 

def sendImgType(arduino):






def main():



arduino = serial.Serial()
arduino.port = 'COM11'
arduino.baudrate = 115200
arduino.timeout = 1
arduino.open()

try: 
    main(arduino)
except KeyboardInterrupt:
    print("\nkeyboard is interrupted")
    print("program stopped.")
    arduino.close()
except Exception as e:
    print("An error occur")
    print(e)
    arduino.close()

