import serial
import time
import os
import numpy as np
import cv2
from matplotlib import pyplot as plt
from my_image import MyImage
import traceback

def getLatestFile(path):
    files = os.listdir(path)
    return files[-1]

def getLatesImgType():
    # get latest image
    path = 'C:\\out'
    #fileName = getLatestFile(path)
    fileName = "out92.bmp"
    image = cv2.imread(path + "\\" + fileName)

    # reduce noise and convert to gray scale
    blur = cv2.GaussianBlur(image, (5,5), 0)
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
    mapped = mapImg(gray)

    # first threshold for mapped image
    retVal, thresh = cv2.threshold(mapped, 80, 255, cv2.THRESH_BINARY) 

    # second threshold for 24 x 24 image
    dim24 = (24, 24)
    resize24 = cv2.resize(thresh, dim24)
    retValV2, thresh24 = cv2.threshold(resize24, 150, 255, cv2.THRESH_BINARY)

    # final threshold for 4 x 4 image
    dim4 = (4,4)
    resize4 = cv2.resize(thresh24, dim4)
    ret, thresh4 = cv2.threshold(resize4, 150, 255, cv2.THRESH_BINARY)

    img24 = thresh24
    img4 = thresh4

    return img24, img4

def sendImgType(arduino, byte_type):
    time.sleep(0.1)
    print("sending img byte to arduino")
    arduino.write(byte_type.encode('ascii'))
    while True:
        if arduino.inWaiting():
            raw = arduino.read_until()
            print("data from arduino: ", end="")
            print(raw.decode("ascii"))
            arduino.close()
            break

def main(arduino):
    img24, img4 = getLatesImgType()
    dim = (24, 24)
    images = MyImage(dim)

    listResults, dictResults, predicted = images.predict(img24, int(0.7 * img24.size))
    #print(listResults)
    #print(dictResults)
    print("==== predicted result ===")
    print(predicted)

    # char for sending to arduino

    dictBytes = {
        "top": '1',
        "bottom": '2',
        "left": '3',
        "right": '4',
        "upper": '5',
        "lower": '6'
    }

    sendImgType(arduino, dictBytes[predicted])

try: 
    arduino = serial.Serial()
    arduino.port = 'COM11'
    arduino.baudrate = 115200
    arduino.timeout = 1
    arduino.open()
    time.sleep(2)
    main(arduino)
except KeyboardInterrupt:
    print("\nkeyboard is interrupted")
    print("program stopped.")
except Exception:
    print("An error occurred")
    print(traceback.print_exc())
finally:
    arduino.close()
