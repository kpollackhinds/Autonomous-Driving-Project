# Autonomous-Driving-Project
Object detection and path tracking using ESP32 CAM and Wifi Video Streaming

## Objective
The goal of this project is to develop an autonomous vehicle that leverages remote computation resources in order to make decisions based on perception. Features will include object detection, path following, and obstacle avoidance.

A primary focus is the development of deep learning methods for the conversion of camera input to motor commands for path following.

## System Layout 
![Alt Text](https://github.com/kpollackhinds/Autonomous-Driving-Project/blob/main/system%20layout.png?raw=true)

### System Description
All communications are perfomed through a Flask based webserver hosted on a laptop. Camera feed from the ESP32 is accessed via an IP address and can be used to display on the frontend, as well as perform inference on the backend.

## Parts Used
Different options have been explored for the MCU of the system. Just about any wifi enabled board should be suitable though
- Arduino Nano ESP32 (Previously used boards: Raspberry Pi Pico W, Arduino Uno R4 Wifi)
- ESP32 Wrover with OV2640 Cam
- L293D DC Motor Controller (H-Bridge)
- DC Motors (with Encoders)
- 6V Power source for motors (4 x 1.5V AA Batteries)
- Power source for MCU and ESP32 (Power bank)
