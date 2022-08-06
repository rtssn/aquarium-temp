# esp32リセットツール

import serial
import time

COM = "COM3"
bitRate = 9600
ser = serial.Serial(COM, bitRate, timeout=0.1)

ser.setDTR(False)
time.sleep(0.1)
ser.setRTS(False)
ser.rtscts = False
time.sleep(1)  # wait esp32 wakeup

# your code here

ser.close()
