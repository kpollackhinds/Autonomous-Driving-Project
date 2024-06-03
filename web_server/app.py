from flask import Flask, request, render_template, send_from_directory, Response
from flask_socketio import SocketIO
import socket
import threading

import cv2

from time import time
import sys, os
import argparse
sys.path.append(os.path.abspath(os.path.join('..', 'AUTONOMOUS-DRIVING-PROJECT')))

from path_tracking.stream_processing import record_frame, record_commands
from path_tracking.traditional_methods.method1_funcs import followLine

parser = argparse.ArgumentParser()
parser.add_argument('--cam', type=int, default= 1, help= 'True or false for whether camera is attached or not')
args = parser.parse_args()

app = Flask(__name__, template_folder=  'templates', static_folder= 'static_files')
socketio = SocketIO(app)
HOST = "0.0.0.0"
PORT = 5000

connected = False
start_saving = False
save_frame = False
start_deploy = False 
start_trad = False
record_interval = 10
stream_url = 'http://192.168.1.164/stream'  #home
cap = None

global_ret =None
global_frame =None
command_array = []
current_time = time()

counter = 0
base_path = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))  # Get the parent directory
output_image_dir = os.path.join(base_path, 'data')

# if not os.path.exists(output_image_dir):
#     os.makedirs(output_image_dir)  # Create the directory if it doesn't exist

if args.cam == 0:
    pass
else:
    try:
        cap = cv2.VideoCapture(stream_url)
    except Exception as e:
            args.cam == 0
            print("Error capturing frame: {e}".format())


def gen_frames():  
    global start_saving
    global save_frame
    global frame
    global start_trad
    while True:
        success, img = cap.read()  # read the camera frame
        if not success:
            break
        else:
            ret, buffer = cv2.imencode('.jpg', img)
            frame = buffer.tobytes()
            if frame:
                yield (b'--frame\r\n'
                    b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')  # concat frame one by one and show result
            
        
            
        if start_saving and save_frame:
                try:
                    # Save the frame
                    # resized = cv2.resize(img, (30,30))
                    # gray = cv2.cvtColor(resized, cv2.COLOR_BGR2GRAY)
                    # frame_to_save = gray

                    frame_to_save = img
                    base_path = os.path.abspath(os.path.join(os.path.dirname(__file__),'..'))
                    output_img_dir = os.path.join(base_path, 'data')

                    if not os.path.exists(output_image_dir):
                        os.makedirs(output_image_dir)

                    num = 0
                    path = os.path.join(output_image_dir, f'image{num}.jpg')

                    #     num+=1
                    #     path = os.path.join(output_image_dir, f'image{num}.jpg')
                    # path = f'{base_path}{num}.jpg'

                    while os.path.exists(path):
                        num += 1
                        path = os.path.join(output_image_dir, f'image{num}.jpg')

                    # print(path)
                    success = cv2.imwrite(filename=path, img =cv2.flip(frame_to_save, 0))
                    if success:
                        print(f'Saved image at {path}')
                        save_frame = False
                    else:
                        print('Failed to save image')
                except Exception as e:
                    print(f'Error saving frame: {e}')
        elif start_trad:
            gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        #     # PUT STUFF HERE
            _, thresholded = cv2.threshold(gray, 70, 255, cv2.THRESH_BINARY)

            contours, hierachy = cv2.findContours(thresholded, 1, cv2.CHAIN_APPROX_NONE)

            if contours:
                # Find the largest contour and its center
                c = max(contours, key=cv2.contourArea)
                M = cv2.moments(c)
                if M["m00"] != 0:
                    cx = int(M["m10"] / M["m00"])
                    cy = int(M["m01"] / M["m00"])
                                    
                # Compute deviation from the center of the frame
                deviation = cx - (320 // 2)
                speed_base = 90
                speed_max = 180
                adjustment_factor = 0.5

                # Adjust motor speeds based on deviation
                left_speed = min(speed_base - (deviation * adjustment_factor), speed_max)
                right_speed = min(speed_base + (deviation * adjustment_factor), speed_max)

                send_data(f"{left_speed},{right_speed}\n")




def send_data(data):
    if 'mcu_socket' in globals():
        try:
            print(data)
            mcu_socket.sendall(data.encode('utf-8'))
        except Exception as e:
            print(f'Error sending data to Pico: {e}')
    return

@socketio.on('pico_connection')
def mcu_connection_handler():
    global mcu_socket
    global connected
    global start_saving
    global save_frame
    global counter
    
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.bind((HOST, PORT))
        server_socket.listen()

        print(f"Waiting for MCU Client to connect on {HOST}:{PORT}")
        mcu_socket, addr = server_socket.accept()
        connected = True
        socketio.emit('mcu_connected')
        print('mcu connected')
        while True:
            full_data = ''
            while True:
                data = mcu_socket.recv(1024)
                if not data:
                    break

                full_data += data.decode('utf-8')
                if full_data.endswith('\n'):
                    break
            
            full_data = full_data[:-1]

            if start_saving:
                # Velocities will be sent with a leading "_" 
                if full_data[0] == '_' and counter >0:
                        save_frame = True
                        command_array.append(full_data[1:])
                counter+=1
            print(full_data)

  
@app.route('/video_feed')
def video_feed():
    if not args.cam:
        return
    global start_saving

    return Response(gen_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

@app.route('/')
def controller():
    return render_template("index.html")

@app.route('/train')
def train():
    return render_template("train.html")

@socketio.on('button_click')
def button_click(data):
    print(data['data'])
    if connected:
        send_data(data['data'])


@socketio.on('manage_record')
def manage_record(data):
    global start_saving
    global command_array

    print(data['data'])
    if data['data'] == "start":
        print('starting to record')
        start_saving = True

        if connected:
            send_data('strt\n')
        else:
            print("Device not connected, stop and try again")


    elif data['data'] == "stop":
        start_saving = False
        print('ending recording')
        if connected:
            send_data('stpt\n')
        record_commands(command_array)
        command_array = []

@socketio.on('start_method')
def manage_method(data):
    data = data["data"]
    global start_deploy
    global start_trad
    match data:
        case "start_model1": 
            start_trad = False
            start_deploy = True
            # initialize model here by readinig it in from the  file
            print("model 1")
            pass
        case "start_mobilenet":
            start_trad = False
            start_deploy = True
            # initialize model here by readinig it in from the  file
            print("mobilenet")
            pass
        case "stop_motion":
            start_trad = False
            start_deploy = False
            print("stopping everythin")
            send_data("stpt\n")
            pass
        case "start_trad":
            start_deploy = False
            start_trad = True
            # need to pass some message to the vehicle to make it stop
            if connected:
                # send_data("stpt\n")
                send_data("strt\n")
                send_data("trad\n")
            print("starting contour method")
            pass
        case _:
            print("invalid data:\n")
            print(data)

@socketio.on('joystick_move')
def joystick_move(data):
    print(data['data'])
    if connected:
        send_data(data['data'])

@app.route('/js/index.js')
def serve_js(filename):
    return send_from_directory('static_files/js', filename)

@socketio.on('start_mcu_thread')
def start_mcu_thread():
    print('starting connection')
    mcu_thread = threading.Thread(target=mcu_connection_handler)
    mcu_thread.daemon = True
    mcu_thread.start()


if __name__ == '__main__':
    # temporary method of dealing with startup error when camera is not attached. Add opencv error handling later!
    if args.cam == 1:
        print(args.cam)
    socketio.run(app, host= '0.0.0.0', port= 8080, debug = True)