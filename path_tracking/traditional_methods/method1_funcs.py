import cv2
import numpy as np


def followLine(frame):
    # frame = cv2.flip(frame, -1)
    # Convert to grayscale and blur
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # Threshold the image
    _, thresholded = cv2.threshold(gray, 40, 255, cv2.THRESH_BINARY)
    
    # Convert the binary thresholded image to a BGR image so we can draw colored elements on it
    colored_thresholded = cv2.cvtColor(thresholded, cv2.COLOR_GRAY2BGR)
    
    # Find contours
    # contours, hierachy = cv2.findContours(thresholded, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)    
    contours, hierachy = cv2.findContours(thresholded, 1, cv2.CHAIN_APPROX_NONE)

    cv2.drawContours(frame, contours, -1, (0,255,0), 2)
    # Draw contours on the colored thresholded image

    if contours:
        # Find the largest contour and its center
        c = max(contours, key=cv2.contourArea)
        M = cv2.moments(c)
        if M["m00"] != 0:
            cx = int(M["m10"] / M["m00"])
            cy = int(M["m01"] / M["m00"])
            
            # Display the center
            cv2.circle(colored_thresholded, (cx, cy), 5, (0, 255, 0), -1)  # Green color for visibility
        
        # Compute deviation from the center of the frame
        deviation = cx - (320 // 2)
        speed_base = 100
        speed_max = 180
        adjustment_factor = 0.5
        cv2.drawContours(colored_thresholded, c, -1, (0, 0, 255), 2)  # Red color for visibility

        # Adjust motor speeds based on deviation
        left_speed = min(speed_base - (deviation * adjustment_factor), speed_max)
        right_speed = min(speed_base + (deviation * adjustment_factor), speed_max)
        
      
    return colored_thresholded, left_speed, right_speed
   
   
