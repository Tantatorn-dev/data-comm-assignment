import os
import numpy as np
import cv2
import math
from matplotlib import pyplot as plt
from my_image import MyImage


def get16(img):
    cor16 = np.array([
         [25, 20, 0], [25,40, 0], [25, 60, 0], [25, 80, 0],
        [55, 20, 0], [55,40, 0], [55, 60, 0], [55, 80, 0],
        [105, 20, 0], [105,40, 0], [105, 60, 0], [105, 80, 0],
        [135, 20, 0], [135,40, 0], [135, 60, 0], [135, 80, 0],
        ])

    pointsLength = cor16.shape[0]
    for i in range(pointsLength):
        y = cor16[i][0]
        x = cor16[i][1]
        cor16[i][2] = img[y][x]
    return cor16

def preProcessImage(image):
    global imgStacks
    # reduce noise

    blur = cv2.GaussianBlur(image, (5, 5), 0)
    imgStacks.append(("blur", blur))

    gray = cv2.cvtColor(blur, cv2.COLOR_BGR2GRAY)

    # mapping useful for too low and too high brightness image
    def mapImg(img):
        scale = np.amax(img) - np.amin(img)
        rows, cols = img.shape
        newImg = np.zeros((rows, cols), np.uint8)
        for r in range(rows):
            for c in range(cols):
                newVal = np.uint8(((img[r][c]-np.amin(img)) / scale) * 255)
                newImg[r][c] = newVal
        return newImg

    mapped = mapImg(gray)
    imgStacks.append(("after mapping", mapped))

    # second threshold for 24 x 24 image
    dim24 = (24, 24)
    resize24 = cv2.resize(mapped, dim24)
    imgStacks.append(("resize24x24", resize24))

    retValV2, thresh24 = cv2.threshold(resize24, 80, 255, cv2.THRESH_BINARY)
    imgStacks.append(("thresh24", thresh24))

    # final threshold for 4 x 4 image
    dim4 = (4, 4)
    resize4 = cv2.resize(thresh24, dim4)
    imgStacks.append(("resize4x4", resize4))

    ret, thresh4 = cv2.threshold(resize4, 150, 255, cv2.THRESH_BINARY)
    imgStacks.append(("thresh4", thresh4))

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



path = 'C:\\out\\'

fileName = input("Enter file name(.jpg): ") + ".jpg"
originalImage = cv2.imread(path+fileName)
imgStacks = []
imgStacks.append(("Original", originalImage))

img24, img4 = preProcessImage(originalImage)

myImg = MyImage(img24.shape)
listResults, dictResults, predictedImage = myImg.predict(img24, 60)
print("Image found is", predictedImage)

if predictedImage != "error":
    labelImg = "perfect " + predictedImage.capitalize() + " image24x24"
    perfectImg = myImg.getPerfect(predictedImage)
    imgStacks.append((labelImg, perfectImg))


# pyplot give number of rows, columns and index
cols = 3
rows = math.ceil(len(imgStacks) / cols)
pos = (rows*100) + (cols*10)

for i in range(len(imgStacks)):
    plt.subplot(pos+i+1),plt.imshow(imgStacks[i][1], cmap = 'gray')
    plt.title(imgStacks[i][0]), plt.xticks([]), plt.yticks([]) 

plt.show()
