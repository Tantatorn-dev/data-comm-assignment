import os
import numpy as np
import cv2
import math
from matplotlib import pyplot as plt
from my_image import MyImage


path = 'C:\\out'

fileName = input("Enter file name(.jpg): ") + ".jpg"

imgStacks = []

image = cv2.imread((path + "\\" + fileName), 0)

imgStacks.append(("original", image))

# reduce noise
blur = cv2.GaussianBlur(image, (5,5), 0)
imgStacks.append(("blur", blur))

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
imgStacks.append(("after mapping", mapped))


# second threshold for 24 x 24 image
dim24 = (24, 24)
resize24 = cv2.resize(mapped, dim24)
imgStacks.append(("resize24x24", resize24))


retValV2, thresh24 = cv2.threshold(resize24, 80, 255, cv2.THRESH_BINARY)
imgStacks.append(("thresh24x24", thresh24))

# final threshold for 4 x 4 image
dim4 = (4,4)
resize4 = cv2.resize(thresh24, dim4)
imgStacks.append(("resize4x4", resize4))

ret, thresh4 = cv2.threshold(resize4, 150, 255, cv2.THRESH_BINARY)
imgStacks.append(("thresh4x4", thresh4))

img24 = thresh24
img4 = thresh4


# Predicting image
images = MyImage(dim24)

print("=== prediction results ===")
results, key, predicted = images.predict(thresh24, int(0.8 * thresh24.size))
print(results)
print(key)
print("The image found is", end=" ")
print(predicted)

if predicted != "error":
    perfectImg = images.getPerfect(predicted)
    perfect = "prefect " + predicted.capitalize() + " image24x24"
    imgStacks.append((perfect, perfectImg))

# pyplot give number of rows, columns and index
cols = 3
rows = math.ceil(len(imgStacks) / cols)
pos = (rows*100) + (cols*10)

for i in range(len(imgStacks)):
    plt.subplot(pos+i+1),plt.imshow(imgStacks[i][1], cmap = 'gray')
    plt.title(imgStacks[i][0]), plt.xticks([]), plt.yticks([]) 

plt.show()
