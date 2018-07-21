#!/usr/bin/python

import serial , string, time

output = " " 
#need to account for fact that the comport can change
try:
    ser = serial.Serial('/dev/ttyACM0', 9600, 8, 'N', 1, timeout=1)
except:
    ser = serial.Serial('/dev/ttyACM1', 9600, 8, 'N', 1, timeout=1)

counter = 0 
#values = ['b','b', 'b','b', 'b','b', 'b','b','b','b']
values = [1,0,1,0,1,1,0,1,0,1]
#while True:
while counter < 10:
        #while output != "":
    val = values[counter]
    val = bytes([val])
    #val = val.encode()
    num = ser.write(val)
    print("Bytes sent:",val)
    #parse the data
    # output will terminate after a newline char from arduino
    # newline char in arduino is given by "\r" 
    time.sleep(5)


    #val = 0
    #val =bytes([val])
    #ser.write(val)
    counter +=1
    output = ser.readline()
    print("Output:",output)
#ser.write('3')
