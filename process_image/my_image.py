import numpy as np

class MyImage:
    def __init__(self, dim):
        self.dim = dim
        self.pixel = dim[0] * dim[0]
        self.lable = ['top', 'bottom', 'left', 'right', 'upper', 'lower']
        self.image = []
        for i in range(6):
            tempImg = self.gen6Img(dim, self.lable[i])
            self.image.append(tempImg)

    def getOriginal(self, img_lable):
        orig = "Not found"
        for i in range(6):
            if self.lable[i] == img_lable:
                orig = self.image[i]
        return orig

    def predict(self, img, ERROR_THRESHOLD):
        listResults = []
        dictResults = {}
        for i in range(6):
            print(self.lable[i], end=": ")
            matches = self.imgCheck(img, self.image[i])
            listResults.append(matches)
            dictResults[self.lable[i]] = matches
            percent = "(" + str(int((matches / self.pixel) * 100)) + "%)"
            print("matches", matches, "of", str(self.pixel), percent)
        maxVal = max(listResults)
        maxi = listResults.index(maxVal)
        predicted_image = self.lable[maxi]
        if maxVal < ERROR_THRESHOLD:
            predicted_image = "error"
        return listResults, dictResults, predicted_image


    def imgCheck(self, img1, img2):
        lenght = img1.shape[0]
        matches = 0
        for y in range(lenght):
            for x in range(lenght):
                if img1[y][x] == img2[y][x]:
                    matches += 1;
        return matches

    def gen6Img(self, dimention, type):
        buff = np.zeros(dimention, np.uint8)
        row = dimention[0]
        col = dimention[1]
        half = row // 2
        for r in range(row):
            for c in range(col):
                check = False
                if type == "top":
                    if r+1 <= half:
                        check = True
                if type == "bottom":
                    if r+1 > half:
                        check = True
                if type == "left":
                    if c+1 <= half:
                        check = True
                if type == "right":
                    if c+1 > half:
                        check = True
                if type == "upper":
                    if c >= r:
                        check = True
                if type == "lower":
                    if r >= c:
                        check = True
                if check:
                    buff[r][c] = 255
        return buff