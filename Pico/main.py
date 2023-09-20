import machine
from machine import Pin
from time import sleep
import usocket as socket
import ujson
import network

from servo import Servo, Controls
from ws_connection import connectWifi, connect_to_socket
from secrets import Secrets

led = Pin("LED", Pin.OUT, value=0)
rightServo = Servo(pin=15)
leftServo = Servo(pin=2 )
c = Controls(rightServo, leftServo)

secrets = Secrets()
ssid, pw = secrets.ssid, secrets.pw
global wlan
wlan = network.WLAN(network.STA_IF)

while True:
    sock = connect_to_socket(ssid, pw, wlan, secrets.server_ip, secrets.port)
    if sock is not None:
        led.on()
        while True:
            try:
                data = sock.recv(1024)
                if not data:
                    break
                
                if data == b'stp':
                    c.stop()
                elif data == b'frwd':
                    c.forward()
                elif data == b'rght':
                    c.right()
                elif data == b'lft':
                    c.left()
                elif data == b'bck':
                    c.backward()

                print(data)
            except OSError as e:
                #handle socket erros or connection issues
                print("Socket error: ", e)
                sock.close()
                wlan.disconnect()
                wlan.active(False)
                led.off()
                break
            except KeyboardInterrupt:
                print('interrupt')
                sock.close()
                wlan.disconnect()
                wlan.active(False)
                led.off()
                break

sock.close()
wlan.disconnect()
wlan.active(False )
sleep(3)
machine.reset()