import cv2
from cv2 import dnn
from ultralytics import YOLO


url = 'http://192.168.1.164/stream'

cap = cv2.VideoCapture(url)

#creating model object with weights
model = YOLO("/weights/yolov8n.pt")

classNames = ["person", "bicycle", "car", "motorbike", "aeroplane", "bus", "train", "truck", "boat",
              "traffic light", "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat",
              "dog", "horse", "sheep", "cow", "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella",
              "handbag", "tie", "suitcase", "frisbee", "skis", "snowboard", "sports ball", "kite", "baseball bat",
              "baseball glove", "skateboard", "surfboard", "tennis racket", "bottle", "wine glass", "cup",
              "fork", "knife", "spoon", "bowl", "banana", "apple", "sandwich", "orange", "broccoli",
              "carrot", "hot dog", "pizza", "donut", "cake", "chair", "sofa", "pottedplant", "bed",
              "diningtable", "toilet", "tvmonitor", "laptop", "mouse", "remote", "keyboard", "cell phone",
              "microwave", "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase", "scissors",
              "teddy bear", "hair drier", "toothbrush"
              ]


#check if camera open successfully
if (cap.isOpened() == False):
    print("Error opening video on stream")
    exit()

while(cap.isOpened()):
    ret, frame = cap.read()
    if ret == True:
        results = model(frame, stream = True)

        for r in results():
            boxes = r.boxes

            for box in boxes:
                x1, y1, x2, y2 = box.xyxy[0]
                x1, y1, x2, y2 = int(x1), int(y1), int(x2), int(y2)

                cv2.rectangle(frame, (x1, y1), (x2, y2), color =(255, 0, 255), thickness=3)

                conf = box.conf[0]

            conf = data[4]
        cv2.imshow('ESP32 Wrover Cam Stream', cv2.flip(frame, 0))
        
        if cv2.waitKey(25) & 0xFF == ord('q'):
            break
    else:
        break

cap.release()
cv2.destroyAllWindows()
