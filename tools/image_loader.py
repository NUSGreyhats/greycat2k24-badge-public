'''
Image loader

Put the badge in USB mode before running this python script
It loads an image onto the badge that can be displayed.
'''

from PIL import Image
import numpy as np
import time
image = Image.open("image.png")
image = image.resize((128, 64), Image.LANCZOS)
#image.show()
input_array = np.asarray(image)
print(image.size)
print(input_array.shape)
#output_array = do_my_stuff(input_array)
imagedata = []

for col in range(128): # x
    value = 0
    for row in range(64): # y
        #if (col%2==0 and row%2==0): #> 64):
        if (input_array[row][col][0] == 255):# and not(54 < col < 64)):
            value += 2**row
    imagedata.append(value)


# https://stackoverflow.com/questions/36468530/changing-pixel-color-value-in-pil
img = Image.new(mode="RGB", size=(128, 64))
pixels = img.load()
for col in range(img.size[0]): # for every pixel:
    value = imagedata[col]
    for row in range(img.size[1]):
        condition = value % 2
        if condition == 1:
            pixels[col,row] = (255, 255,255)
        else:
            pixels[col,row] = (0, 0 ,0)
        value //= 2
img.show()

#exit()
import serial
ser = serial.Serial('COM4')  # open serial port
ser.baudrate = 115200

ser.write(f"w\n".encode())

for value in imagedata:
    print(value, end='')
    ser.write(f"{value}\n".encode())
    #print(ser.read_until('\n'))
    print('')
    time.sleep(0.01)

print("done")
ser.write(f"0\n".encode())
ser.write(f"0\n".encode())
ser.write(f"0\n".encode())
ser.write(f"y\n".encode())
#for col in range(128):
#    ser.write(f"0\n".encode())
#ser.write(f"y\n".encode())
ser.close()


'''
print(ser.name)         # check which port was really used
for i in range(64): #64): # x
    for j in range(48): #(24, 48): # y
        on = 'B'
        off = 'C'
        state = off
        if (input_array[j][85-64-1 + i] == 255): state = on
        #if (j == 1): state = on
        ser.write((state + chr(j) + chr(i) + 'A').encode())     # write a string

'''

# ser.close()             # close port
