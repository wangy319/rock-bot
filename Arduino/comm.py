#!/usr/bin/python

import serial , string, time

output = " " 
#need to account for fact that the comport can change
try:
    ser = serial.Serial('/dev/ttyACM0', 9600, 8, 'N', 1, timeout=1)
except:
    ser = serial.Serial('/dev/ttyACM1', 9600, 8, 'N', 1, timeout=1)
counter = 0 
temp, humid = 0, 0

#while True:
while counter < 10:
    print "----"
    #while output != "":
    output = ser.readline()
    #parse the data
    # output will terminate after a newline char from arduino
    # newline char in arduino is given by "\r" 
    bug = True
    val = ""
    while bug:
        try:
            val = float(output.split("= ")[1].split("\r")[0])
            bug =  False
        except:
            # keep appending to val
            temp = ser.readline()
            print("Old val", val)
            val = val + temp
            print("Temp", temp)

    if output[0] == "T" : #its a temp
        temp = val
    elif output[0] == "H": # its humidity
        humid = val
        
    print "Temp", temp, "Humidity", humid
    count = str(counter %3)
    ser.write(count)
    time.sleep(3)
    output = " "
    counter +=1
#ser.write('3')
