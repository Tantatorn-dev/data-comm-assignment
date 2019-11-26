import os

import numpy as np
import cv2


def get16(img):
    cor16 = np.array([
         [25, 20, 0], [25,40, 0], [25, 60, 0], [25, 80, 0],
        [55, 20, 0], [55,40, 0], [55, 60, 0], [55, 80, 0],
        [105, 20, 0], [105,40, 0], [105, 60, 0], [105, 80, 0],
        [135, 20, 0], [135,40, 0], [135, 60, 0], [135, 80, 0],
        ])

    pointsLength = cor16.shape[0]
    print(pointsLength)
    for i in range(pointsLength):
        y = cor16[i][0]
        x = cor16[i][1]
        cor16[i][2] = img[y][x]
    return cor16

img = cv2.imread("C:\\out\\2.jpg", 0)

print(get16(img))