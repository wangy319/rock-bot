import serial

output = " "

try:
    ser=serial.Serial('/dev/ttyACMO',9600,8,'N',1, timeout =1)
