import cv2
import numpy as np

url = 'http://192.168.1.164/stream'  #home

cap = cv2.VideoCapture(url)

while cap.isOpened() == False:
    ret, frame = cap.read()
    if ret:

        #getting a gray scaled image
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

        edges = cv2.Canny(gray, 50, 50, apertureSize=3)

        lines = cv2.HoughLines(edges, 1, np.pi/180,200)
        #image smoothing
        # blurred = cv2.GaussianBlur(gray, (9,9), 0)
        cv2.imshow(frame)

        if cv2.waitKey(25) & 0xFF == ord('q'):
            break
    else:
        break

cap.release()
cv2.destroyAllWindows()