import cv2
import time 
import os
import numpy as np
# def capture_frames(cap, save_images, stream_url = None, record_interval = 10):
#     if not stream_url:
#         return
    
#     if not record_interval:
#         record_interval = 10
    
#     current_time = time.time()
    

#     while True:
#         ret, frame = cap.read()

#         ret, buffer = cv2.imencode('.jpg', frame)
#         frame = buffer.tobytes()

#         yield (b'--frame\r\n'
#                 b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')  # concat frame one by one and show result
        
#         if save_images and time.time() - current_time >= record_interval:
#             record(ret, frame)
        

        
def capture_frames(cap):
    # if not stream_url:
    #     return
    
    # if not record_interval:
    #     record_interval = 10
    
    # current_time = time.time()
    

    while True:
        ret, frame = cap.read()

        ret, buffer = cv2.imencode('.jpg', frame)
        frame = buffer.tobytes()

        yield (b'--frame\r\n'
                b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')  # concat frame one by one and show result
        
        # if save_images and time.time() - current_time >= record_interval:
        #     record(ret, frame)
        

def record_frame(ret, frame):
    if ret:
        resized = cv2.resize(frame, (30,30))
        gray = cv2.cvtColor(resized, cv2.COLOR_BGR2GRAY)

        path = '../data/output_image0'

        if os.path.exists(path + '.jpg'):
            path[-1] = str(int(path[-1]) +1) + '.jpg'

        cv2.imwrite(path, cv2.flip(gray, 0))
        print('saved image')
    

def record_commands(velocity_array):
    formatted_vel_array = []
    for vels in velocity_array:
        x = vels.split(',')
        formatted_vel_array.append([float(x[0]), float(x[1])])
    
    path = '../data/velocity_array.npy'
    arr = np.array(formatted_vel_array)
    print(arr.shape)
    np.save(formatted_vel_array, path)