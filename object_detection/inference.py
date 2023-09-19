import cv2

url = 'http://192.168.1.164/stream'

cap = cv2.VideoCapture(url)

#check if camera open successfully
if (cap.isOpened() == False):
    print("Error opening video on stream")
    exit()

while(cap.isOpened()):
    ret, frame = cap.read()
    if ret == True:

        cv2.imshow('ESP32 Wrover Cam Stream', cv2.flip(frame, 0))
        
        if cv2.waitKey(25) & 0xFF == ord('q'):
            break
    else:
        break

cap.release()
cv2.destroyAllWindows()
