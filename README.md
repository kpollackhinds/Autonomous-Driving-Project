# Autonomous-Driving-Project
Object detection and path tracking using ESP32 CAM and Wifi Video Streaming

## Objective
The goal of this project is to develop an autonomous vehicle that leverages remote computation resources in order to make decisions based on perception. Features will include object detection, path following, and obstacle avoidance.

A primary focus is the development of deep learning methods for the conversion of camera input to motor commands for path following.

## System Layout 
![image](https://github.com/kpollackhinds/Autonomous-Driving-Project/assets/84944416/dd54ec1e-ce77-4c5b-acd4-9c52f34c6334)

### System Description
All communications are perfomed through a Flask based webserver hosted on a laptop. Camera feed from the ESP32 is accessed via an IP address and can be used to display on the frontend, as well as perform inference on the backend. Manual controls can be performed by interfacing with either direction buttons, or a joystick (obtained from https://github.com/bobboteck/JoyStick)

### Operation
Ensure all packages are installed by using _pip install -r requirements.txt_ (Do this in a virtual environment)

For everything to communicate through the web interface, all devices need to be connected to the same wifi network. I defined my network ssid, password and ip in a header file that I did not put on this repository, but you can also specifify by uncommenting the apporpriate place in the [ESP32 Wrover Sketch](https://github.com/kpollackhinds/Autonomous-Driving-Project/blob/main/ESP32/ESP32_Wrover/Sketch_06.2_As_VideoWebServer/Sketch_06.2_As_VideoWebServer.ino):,
![image](https://github.com/kpollackhinds/Autonomous-Driving-Project/assets/84944416/76c57741-131a-4a2e-855e-2d55293dc023)

and copying and pasting in
```C
#define REMOTE_PORT 5000
const char* ssid = "1234"; // Replace "1234" with your actual SSID
const char* password = "123w4"; // Replace "123w4" with your actual password
const char* ip_address = "***.***.*.***";
```
into the [Arduino Sketch](https://github.com/kpollackhinds/Autonomous-Driving-Project/blob/main/Arduino_wifi/arduino_esp32/main/main.ino).

In order to see the video feed from ESP32 Wrover (or any other ip cam) change the link specified in the src tag of the [index.html file](https://github.com/kpollackhinds/Autonomous-Driving-Project/blob/main/web_server/templates/index.html) to reflect your video source (will be a different ip address): ![image](https://github.com/kpollackhinds/Autonomous-Driving-Project/assets/84944416/6ef488c6-2a26-459e-9082-3bfa2d448ddd)

If all these steps are done, the ESP32 Wrover is powered and **everything is on the same network**, you can run the app.py file and you should see the following output:

![image](https://github.com/kpollackhinds/Autonomous-Driving-Project/assets/84944416/54fe55ed-eae1-4c08-ae96-d354a7def9e0)

Clicking the start button will initialize the connection the MCU board. (Check terminal for confirmation output).


## Parts Used
Different options have been explored for the MCU of the system. Just about any wifi enabled board should be suitable though
- Arduino Nano ESP32 (Previously used boards: Raspberry Pi Pico W, Arduino Uno R4 Wifi)
- ESP32 Wrover with OV2640 Cam
- L293D DC Motor Controller (H-Bridge)
- DC Motors (with Encoders)
- 6V Power source for motors (4 x 1.5V AA Batteries)
- Power source for MCU and ESP32 (Power bank)
