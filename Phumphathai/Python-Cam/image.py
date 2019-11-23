
import os
import cv2
import numpy as np
from my_image import MyImage


def preProcessImage(image):
    # reduce noise
    blur = cv2.GaussianBlur(image, (5, 5), 0)

    # mapping useful for low brightness image
    def mapImg(img):
        scale = np.amax(img) - np.amin(img)
        rows, cols = img.shape
        newImg = np.zeros((rows, cols), np.uint8)
        for r in range(rows):
            for c in range(cols):
                newVal = np.uint8(((img[r][c]-np.amin(img)) / scale) * 255)
                newImg[r][c] = newVal
        return newImg

    mapped = mapImg(blur)

    # second threshold for 24 x 24 image
    dim24 = (24, 24)
    resize24 = cv2.resize(mapped, dim24)

    retValV2, thresh24 = cv2.threshold(resize24, 80, 255, cv2.THRESH_BINARY)

    # final threshold for 4 x 4 image
    dim4 = (4, 4)
    resize4 = cv2.resize(thresh24, dim4)

    ret, thresh4 = cv2.threshold(resize4, 150, 255, cv2.THRESH_BINARY)

    img24 = thresh24
    img4 = thresh4

    return img24, img4


def getDictBytes(key):
    dictBytes = {
        "error": '0',
        "top": '1',
        "bottom": '2',
        "left": '3',
        "right": '4',
        "upper": '5',
        "lower": '6'
    }

    return dictBytes[key]


def getImageData(image):
    print("Processing Image...")
    img24, img4 = preProcessImage(image)
    images = MyImage(img24.shape)
    listResults, dictResults, predicted = images.predict(
        img24, np.uint8(0.7 * img24.size))
    print("image found is ", end="")
    print(predicted)
    typeByte = getDictBytes(predicted)
    print("send back is", typeByte)
    return typeByte
