import numpy as np


class MyImage:
    def __init__(self, dim):
        self.dim = dim
        self.pixel = dim[0] * dim[1]
        self.lable = ['top', 'bottom', 'left', 'right', 'upper', 'lower']
        self.image = []
        for i in range(6):
            tempImg = self.gen6Img(dim, self.lable[i])
            self.image.append(tempImg)

    def getPerfect(self, img_lable):
        per = "Not found"
        for i in range(6):
            if self.lable[i] == img_lable:
                per = self.image[i]
        return per

    def predict(self, img, ERROR_THRESHOLD):
        print("---- prediction results ----")
        listResults = []
        dictResults = {}
        blackTops = 0
        blackBottoms = 0
        for i in range(6):
            print(self.lable[i], end=": ")
            matches, blackTops, blackBottoms = self.imgCheck(
                img, self.image[i])
            listResults.append(matches)
            percent = int((matches / self.pixel) * 100)
            dictResults[self.lable[i]] = percent
            print("matches", matches, "of", str(self.pixel),
                  "(" + str(int((matches / self.pixel) * 100)) + "%)")

        maxVal = max(listResults)
        maxi = listResults.index(maxVal)
        predicted_image = self.lable[maxi]
        if predicted_image == "top" and blackTops > 50:
            predicted_image = "upper"
        elif predicted_image == "bottom" and blackBottoms > 50:
            predicted_image = "lower"

        if dictResults[predicted_image] < ERROR_THRESHOLD:
            predicted_image = "error"
        return listResults, dictResults, predicted_image

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
