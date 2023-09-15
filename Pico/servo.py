from machine import Pin, PWM
from time import sleep

class Servo:

    """ A simple class for controlling a 9g servo with the Raspberry Pi Pico.
 
        Attributes:
 
        minVal: An integer denoting the minimum duty value for the servo motor.
 
        maxVal: An integer denoting the maximum duty value for the servo motor.
    """
     
    def __init__(self, pin: int, minVal = 1000000, maxVal = 2000000, midVal = 1500000):
        
        self.minVal = minVal
        self.midVal = midVal
        self.maxVal = maxVal

        self.pwm = PWM(Pin(pin))
        self.pwm.freq(50)
        self.pwm.duty_ns(self.midVal)

    def go(self, speed):
        if speed == 0:
            self.pwm.duty_ns(self.midVal)
        
        elif speed > 100:
            self.pwm.duty_ns(self.maxVal)
        
        elif speed < -100:
            self.pwm.duty_ns(self.minVal)
        
        else:
            #mapping input to duty cycle time in nano seconds
            self.value = int(((speed - (-100)) / (100 - (-100)) * (self.maxVal - self.minVal)) + self.minVal)
            self.pwm.duty_ns(self.value)

"""
basic controls for servo motions
"""
class Controls:
    def __init__(self, rightServo: Servo, leftServo: Servo) -> None:
        self.rightServo = rightServo
        self.leftServo = leftServo

    #duration in ms
    def forward(self, duration = None, speed = 50):
        self.rightServo.go(speed)
        self.leftServo.go(speed)

    def backward(self, duration = None, speed = -50):
        if speed >0:
            speed = speed *-1

        self.rightServo.go(speed)
        self.leftServo.go(speed)

    #ratio = speed of "passive" wheel (right wheel if turning right, etc)/ speed of active wheel
    #if ratio is 1, they are turning at the same speed in opposite directions
    def right(self, duration = None, ratio = 1.0, active_wheel_speed = 50):
        self.right_speed = int(-1* ratio * active_wheel_speed)
        
        self.rightServo.go(self.right_speed)
        self.leftServo.go(active_wheel_speed)

    def left(self, duration = None, ratio = 1.0, active_wheel_speed = 50):
        self.left_speed = int(-1* ratio * active_wheel_speed)
        
        self.rightServo.go(active_wheel_speed)
        self.leftServo.go(self.left_speed)

    def stop(self):
        self.rightServo.go(0)
        self.leftServo.go(0)
