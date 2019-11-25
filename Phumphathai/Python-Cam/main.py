import time
import os
import shutil

import serial

from camera import Camera
from image import getImageData

folder = './out/'
for the_file in os.listdir(folder):
    file_path = os.path.join(folder, the_file)
    try:
        if os.path.isfile(file_path):
            os.unlink(file_path)
        elif os.path.isdir(file_path):
            shutil.rmtree(file_path)
    except Exception as e:
        print('Failed to delete %s. Reason: %s' % (file_path, e))

ser = serial.Serial()
ser.port = 'COM15'
ser.baudrate = 115200
ser.bytesize = serial.EIGHTBITS
ser.stopbits = serial.STOPBITS_ONE
ser.parity = serial.PARITY_NONE
ser.timeout = 0.4
ser.open()

cam = Camera('COM3')
cam.connect()
time.sleep(2)

i = 0
while True:
    while not ser.inWaiting():
        time.sleep(0.2)
    operation = ser.read_until().decode('ascii')
    if (operation[0] != 'D'):
        operation = operation[0]
        print("Ser", operation, operation == 'c')
        Err = True
        while Err:
            img, Err = cam.getImg()
            cam.display(img)
            if not Err:
                typeByte, cor16 = getImageData(img)

                if operation == 'c':
                    ser.write(typeByte.encode('ascii'))
                    print("out for c")
                elif operation == 'x':
                    if typeByte != '0':
                        l = []
                        for i in range(cor16.shape[0]):
                            for j in range(cor16.shape[1]):
                                l.append(cor16[i][j])
                        ser.write(bytearray(l))
                    else:
                        ser.write(bytearray([0]))
                    print("out for x")

                cam.save(img, 'out/' + str(i) + '.png')
                i = i+1
    else:
        print("Ser Debug", operation)
