import os
import numpy as np
import cv2
from matplotlib import pyplot as plt
from my_image import MyImage


path = 'C:\\out'

fileName = input("Enter file name(.bmp): ")

image = cv2.imread(path + "\\" + fileName + ".bmp")
print('original image shape' + str(image.shape))

blur = cv2.GaussianBlur(image,(5,5),0)
rotated = cv2.rotate(blur, cv2.ROTATE_180)
gray = cv2.cvtColor(rotated, cv2.COLOR_BGR2GRAY)

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

mapped = mapImg(gray)

retVal, thresh = cv2.threshold(mapped, 80, 255, cv2.THRESH_BINARY)

x = 24
dim = (x, x)
resize24 = cv2.resize(thresh, dim)
retValV2, thresh24 = cv2.threshold(resize24, 120, 255, cv2.THRESH_BINARY)

dim4 = (4,4)
resize4 = cv2.resize(thresh24, dim4)

ret, thresh4 = cv2.threshold(resize4, 150, 255, cv2.THRESH_BINARY)


# Predicting image
images = MyImage(dim)

print("=== prediction results ===")
results, key, predicted = images.predict(thresh24, int(0.7 * thresh24.size))
print(results)
print(key)
print("The image found is", end=" ")
print(predicted)

if predicted != "error":
    orig = images.getOriginal(predicted)
    

# pyplot give number of rows, columns and index
rows = 3
cols = 3
pos = (rows*100) + (cols*10)

plt.subplot(pos+1),plt.imshow(image, cmap = 'gray')
plt.title('original'), plt.xticks([]), plt.yticks([])

plt.subplot(pos+2),plt.imshow(rotated, cmap = 'gray')
plt.title('rotated'), plt.xticks([]), plt.yticks([])

plt.subplot(pos+3),plt.imshow(mapped, cmap = 'gray')
plt.title('after mapping'), plt.xticks([]), plt.yticks([])

plt.subplot(pos+4),plt.imshow(thresh, cmap = 'gray')
plt.title('thresh'), plt.xticks([]), plt.yticks([])

plt.subplot(pos+5),plt.imshow(resize24, cmap = 'gray')
plt.title('resize24x24'), plt.xticks([]), plt.yticks([])
 
plt.subplot(pos+6),plt.imshow(thresh24, cmap = 'gray')
plt.title('thresh24x24'), plt.xticks([]), plt.yticks([])

plt.subplot(pos+7),plt.imshow(resize4, cmap = 'gray')
plt.title('resize4x4'), plt.xticks([]), plt.yticks([])

plt.subplot(pos+8),plt.imshow(thresh4, cmap = 'gray')
plt.title('thresh4x4'), plt.xticks([]), plt.yticks([])

if predicted != "error":
    perfect = "prefect " + predicted.capitalize() + " image24x24"
    plt.subplot(pos+9),plt.imshow(orig, cmap = 'gray')
    plt.title(perfect), plt.xticks([]), plt.yticks([])

plt.show()
