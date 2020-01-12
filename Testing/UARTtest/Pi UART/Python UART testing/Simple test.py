import serial
ser = serial.Serial("/dev/ttyAMA0",38400)
ser.write('r')
ser.close()
