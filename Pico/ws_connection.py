from time import sleep
import network

def connectWifi(ssid, pw, wlan):
    print('test')
    wlan.active(True)
    wlan.disconnect()
    wlan.connect(ssid, pw)
    while wlan.isconnected() == False:
        print('Waiting for connection...')
        sleep(1)

    ip = wlan.ifconfig()[0]
    print(f'connected on {ip}')
    return ip