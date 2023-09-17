from time import sleep
import network

def connect(ssid, pw):
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect(ssid, pw)
    while wlan.isconnected() == False:
        print('Waiting for connection...')
        sleep(1)

    ip = wlan.ifconfig()[0]
    print(f'connected on {ip}')
    return ip