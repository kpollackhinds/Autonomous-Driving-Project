from machine import Pin
from servo import Servo, Controls
from time import sleep


rightServo = Servo(pin=15)
leftServo = Servo(pin=2 )

c = Controls(rightServo, leftServo)

while True:
    c.forward()
    sleep(2)
    c.backward()
    sleep(2)
    c.right()
    sleep(2)
    c.left()
    sleep(7)