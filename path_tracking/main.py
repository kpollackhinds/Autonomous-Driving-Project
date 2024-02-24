import cv2
import numpy as np

url = 'http://192.168.1.164/stream'  #home

cap = cv2.VideoCapture(url)

#check if camera open successfully
if (cap.isOpened() == False):
    print("Error opening video on stream")
    exit()

# //     FRAMESIZE_HVGA,     // 480x320

while cap.isOpened():
    ret, frame = cap.read()
    if ret:

        #getting a gray scaled image
        # gray = cv2.cvtColor(cv2.flip(frame,0), cv2.COLOR_BGR2GRAY) 
        # gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY) 

        # resized_frame = cv2.resize(gray, (30, 20))
        flipped_frame = cv2.flip(frame,0)
        cv2.imshow('test', flipped_frame)


        # press q to exit
        if cv2.waitKey(3) & 0xFF == ord('q'):
            break

        if cv2.waitKey(3) & 0xFF == ord('k'):
            cv2.imwrite('output_image_reg.jpg', flipped_frame)
            print('got it')

        if cv2.waitKey(3) & 0xFF == ord('s'):
            resized = cv2.resize(flipped_frame, (30,30))
            gray = cv2.cvtColor(resized, cv2.COLOR_BGR2GRAY) 
            cv2.imwrite('output_image.jpg', gray)
            print('got it')

        

    else:
        break

cap.release()
cv2.destroyAllWindows()