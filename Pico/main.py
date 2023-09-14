from machine import Pin
from servo import Servo
from time import sleep
servo1 = Servo(pin=15)

while True:
    servo1.go(20)
    sleep(1)
    servo1.go(-20)
    sleep(1)
    servo1.go(0)
    sleep(7)