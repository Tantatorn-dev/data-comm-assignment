import numpy as np


class MyImage:
    def __init__(self, dim):
        self.dim = dim
        self.pixel = dim[0] * dim[1]
        self.label = ['top', 'bottom', 'left', 'right', 'upper', 'lower']
        self.image = []
        for i in range(6):
            tempImg = self.gen6Img(dim, self.label[i])
            self.image.append(tempImg)

    def getPerfect(self, img_label):
        per = "Not found"
        for i in range(6):
            if self.label[i] == img_label:
                per = self.image[i]
        return per

    def predict(self, img, errorThreshold=60):
        print("---- prediction results ----")
        listResults = []
        dictResults = {}
        blackTops = 0
        blackBottoms = 0
        for i in range(6):
            print(self.label[i], end=": ")
            matches, blackTops, blackBottoms = self.imgCheck(
                img, self.image[i])
            listResults.append(matches)
            percent = int((matches / self.pixel) * 100)
            dictResults[self.label[i]] = percent
            print("matches", matches, "of", str(self.pixel),
                  "(" + str(int((matches / self.pixel) * 100)) + "%)")

        maxVal = max(listResults)
        maxi = listResults.index(maxVal)
        predictedImage = self.label[maxi]
        if predictedImage == "top" and blackTops > 50:
            predictedImage = "upper"
        elif predictedImage == "bottom" and blackBottoms > 50:
            predictedImage = "lower"

        if dictResults[predictedImage] < errorThreshold:
            predictedImage = "error"
        return listResults, dictResults, predictedImage

    def imgCheck(self, img1, img2):
        lenght = img1.shape[0]
        matches = 0
        blackTops = 0
        blackBottoms = 0
        for y in range(lenght):
            for x in range(lenght):
                if img1[y][x] == img2[y][x]:
                    matches += 1
                if img1[y][x] == 0 and y < 10:
                    blackTops += 1
                if img1[y][x] == 0 and y > 12:
                    blackBottoms += 1
        return matches, blackTops, blackBottoms

    def gen6Img(self, dimention, type):
        buff = np.zeros(dimention, np.uint8)
        row = dimention[0]
        col = dimention[1]
        half = row // 2
        for r in range(row):
            for c in range(col):
                check = False
                if type == "top" and r+1 <= half:
                    check = True
                if type == "bottom" and r+1 > half:
                    check = True
                if type == "left" and c+1 <= half:
                    check = True
                if type == "right" and c+1 > half:
                    check = True
                if type == "upper" and c >= r:
                    check = True
                if type == "lower" and r >= c:
                    check = True

                if check:
                    buff[r][c] = 255
        return buff
