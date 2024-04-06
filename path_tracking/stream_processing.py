import cv2
import time 
import os
import numpy as np

def record_frame(stream_url):
    print("trying to create Video Capture")
    cap = cv2.VideoCapture(stream_url)
    print("Created Video Capture")
    ret, frame = cap.read()
    if ret:
        resized = cv2.resize(frame, (30,30))
        gray = cv2.cvtColor(resized, cv2.COLOR_BGR2GRAY)

        base_path = '../data/output_image'
        num = 0
        path = f'{base_path}{num}.jpg'

        while os.path.exists(path):
            num += 1
            path = f'{base_path}{num}.jpg'

        # success = cv2.imwrite(path, cv2.flip(gray, 0))
        success = cv2.imwrite('yes.jpg', cv2.flip(gray, 0))

        if success:
            print(f'Saved image at {path}')
            return True
        else:
            print('Failed to save image')
    
    cap.release()
    

def record_commands(velocity_array):
    formatted_vel_array = []
    for vels in velocity_array:
        x = vels.split(',')
        formatted_vel_array.append([float(x[0]), float(x[1])])

    base_path = r'C:\Users\kxfor\OneDrive\Documents\Projects\Autonomous-Driving-Project\data\velocity_run'
    num = 0
    path = f'{base_path}{num}.npy'

    while os.path.exists(path):
        num += 1
        path = f'{base_path}{num}.npy'
    
    # path = '../data/velocity_array.npy'
    # path = r'C:\Users\kxfor\OneDrive\Documents\Projects\Autonomous-Driving-Project\data\velocity.npy'
    # path = '../data'
    new_data = np.array(formatted_vel_array)
    
    print(new_data.shape)
    np.save(path, new_data)

# def record_commands(velocity_array):
#     formatted_vel_array = []
#     for vels in velocity_array:
#         x = vels.split(',')
#         formatted_vel_array.append([float(x[0]), float(x[1])])
    
#     # path = '../data/velocity_array.npy'
#     path = r'C:\Users\kxfor\OneDrive\Documents\Projects\Autonomous-Driving-Project\data\velocity.npy'
#     # path = '../data'
#     new_data = np.array(formatted_vel_array)
#     if os.path.exists(path):
#         current_data = np.load(path)
#         arr = np.vstack((current_data, new_data))
#     else:
#         arr = new_data

#     try:
#         print(current_data.shape)
#     except:
#         pass
#     print(arr.shape)
#     np.save(path, arr)

    


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
        
# def capture_frames():
#     current_time = time.time()
#     while True:
#         try:
#             ret, frame = cap.read()
#             ret, buffer = cv2.imencode('.jpg', frame)
#             frame_buffer = buffer.tobytes()

#             yield (b'--frame\r\n'
#                     b'Content-Type: image/jpeg\r\n\r\n' + frame_buffer + b'\r\n')  # concat frame one by one and show result
            
#             set_frames("set", ret, frame)
#             print('done')
#             # trigger_record(ret, frame)
#         except Exception as e:
#             print("Error captutring frames: {e}".format())
#             continue
        
# def capture_frames(cap):
#     # if not stream_url:
#     #     return
    
#     # if not record_interval:
#     #     record_interval = 10
    
#     # current_time = time.time()
    

#     while True:
#         ret, frame = cap.read()

#         ret, buffer = cv2.imencode('.jpg', frame)
#         frame = buffer.tobytes()

#         yield (b'--frame\r\n'
#                 b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')  # concat frame one by one and show result
        
#         # if save_images and time.time() - current_time >= record_interval:
#         #     record(ret, frame)
##save_images variable not being updated correctly within video capture method. fix later
# def trigger_record(ret, frame, vel):
#     global command_array
#     global current_time 
#     # if save_images and (time.time() - current_time >= record_interval):
#     record_frame(ret, frame)
#     command_array.append(vel[1:].decode('utf-8'))

#     current_time = time.time()