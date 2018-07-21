import time
import serial

ser = serial.Serial('/dev/ttyACM0',9600) #communication on the ACM0 port
s = [0,1]
while True:
	read_serial=ser.readline()
	s[0] = str(int (ser.readline(),16))
	print s[0]
	print read_serial

        ser.write(b'1')

"""To find our which port the UNO is attached to, go to the terminal of PI and enter" ls/dev/tty*. You will have the list of all attached devices on PI. Now connect the ArduinoUno to Raspberry Pi with USB cable and enter the command again. "
