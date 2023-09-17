from machine import Pin
from time import sleep
import usocket as socket
import ujson

from servo import Servo, Controls
from ws_connection import connect
from secrets import Secrets

rightServo = Servo(pin=15)
leftServo = Servo(pin=2 )
c = Controls(rightServo, leftServo)

secrets = Secrets()
ssid, pw = secrets.ssid, secrets.pw
connect(ssid, pw)

#creating socket object and connecting to server hosted on local machine
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_addr = (secrets.server_ip, secrets.port)
sock.connect(server_addr)

while True:
    try:
        data = sock.recv(1024)
        if not data:
            break
        print(data)
    except OSError as e:
        #handle socket erros or connection issues
        print("Socket error: ", e)
        break

sock.close()
    # c.forward()
    # sleep(2)
    # c.backward()
    # sleep(2)
    # c.right()
    # sleep(2)
    # c.left()
    # sleep(2)
    # c.stop()
    # sleep(7)