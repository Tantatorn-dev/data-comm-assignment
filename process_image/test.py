import os

import numpy as np
import cv2

cor16 = np.array([
         [[25, 20, 0], [25,40, 0], [25, 60, 0], [25, 80, 0]],
        [[55, 20, 0], [55,40, 0], [55, 60, 0], [55, 80, 0]],
        [[105, 20, 0], [105,40, 0], [105, 60, 0], [105, 80, 0]],
        [[135, 20, 0], [135,40, 0], [135, 60, 0], [135, 80, 0]],
        ])

rows, cols, _ = cor16.shape

img = cv2.imread("C:\\out\\2.jpg", 0)

for i in range(rows):
    for j in range(cols):
        y = cor16[i][j][0]
        x = cor16[i][j][1]
        cor16[i][j][2] = img[y][x]

print(cor16)
