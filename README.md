# Autonomous-Driving-Project

Object detection and path following using ESP32 CAM and Wifi Video Streaming

Full documentation available here:
- [Spring 2024 Report (Most recent)](https://www.overleaf.com/read/cvtkfzjbzdcq#cf5f8e)
- [Fall 2023 Report (More detailed mechanical and electrical information)](https://www.overleaf.com/read/jpbfzhhsykrk#73abcc)


## Objective

The goal of this project is to develop an autonomous vehicle that leverages remote computation resources in order to make decisions based on perception. Features will include object detection, path following, and obstacle avoidance.

A primary focus is the development of deep learning methods for the conversion of camera input to motor commands for path following. Traditional computer vision methods for line following are also explored.

## System Layout

![image](https://github.com/kpollackhinds/Autonomous-Driving-Project/assets/84944416/dd54ec1e-ce77-4c5b-acd4-9c52f34c6334)

### System Description

All communications are perfomed through a Flask based webserver hosted on a laptop. Camera feed from the ESP32 is accessed via an IP address and can be used to display on the frontend, as well as perform computation on the backend. Manual controls can be performed by interfacing with either direction buttons, or a joystick (obtained from https://github.com/bobboteck/JoyStick)

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

into the any of the [Arduino Sketches](https://github.com/kpollackhinds/Autonomous-Driving-Project/tree/main/Arduino_wifi/arduino_uno_r4_wifi).

Ensure the source of your video stream is correct in the index.html file in order to see the camera feed.

If all these steps are done, the ESP32 Wrover is powered and **everything is on the same network**, you can run the app.py file using _python web_server app.py --cam 1_ to run with the camera or specifying the arg --cam 0 to run if the camera is not connected. You should see the following output:
![image](https://github.com/kpollackhinds/Autonomous-Driving-Project/assets/84944416/530f669b-8f07-4c15-b97f-f0fce1de9e56)

Clicking the "Connect to Device" Button will intiate the socket connection to the MCU. The color of the button will change to green once conneted.

There are two different Arduino Sketches used in the [Arduino R4 Wifi Folder](https://github.com/kpollackhinds/Autonomous-Driving-Project/tree/main/Arduino_wifi/arduino_uno_r4_wifi) for training and path tracking method deployment. Either one automatically handles the connection. The model deployment file as of right now only handle the [traditional line contour based line following method](https://www.overleaf.com/project/660e2ea59c73f2049fa892e0#subsubsection.3.3.1:~:text=3.3.1-,Method%20Development,-The%20traditional%20method).

## Parts Used

Different options have been explored for the MCU of the system. Just about any wifi enabled board should be suitable though

- Arduino Uno R4 Wifi (Previously used boards: Raspberry Pi Pico W, Arduino Nano ESP32)
- ESP32 Wrover with OV2640 Cam
- L293D DC Motor Controller (H-Bridge)
- DC Motors (with Encoders)
- 9V Battery for motors
- Power source for Arduino and ESP32 (Power bank)
- QTR-8A Reflectance Array
