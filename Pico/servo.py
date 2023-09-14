from machine import Pin, PWM

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
