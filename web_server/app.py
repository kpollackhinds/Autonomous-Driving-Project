from flask import Flask, request, render_template, send_from_directory, Response
from flask_socketio import SocketIO
import socket
import threading

import cv2
import time
import sys, os
import argparse
sys.path.append(os.path.abspath(os.path.join('..', 'AUTONOMOUS-DRIVING-PROJECT')))

from path_tracking.stream_processing import record_frame, record_commands

command_array = []

parser = argparse.ArgumentParser()
parser.add_argument('--cam', type=int, default= 1, help= 'True or false for whether camera is attached or not')
args = parser.parse_args()

app = Flask(__name__, template_folder=  'templates', static_folder= 'static_files')
socketio = SocketIO(app)
HOST = "0.0.0.0"
PORT = 5000

connected = False
save_images = False
record_interval = 10
stream_url = 'http://192.168.1.164/stream'  #home
cap = None

if args.cam == 0:
    pass
else:
    try:
        cap = cv2.VideoCapture(stream_url)
    except Exception as e:
            args.cam == 0
            print("Error capturing frame: {e}".format())

current_time = time.time()

def send_data(data):
    if 'mcu_socket' in globals():
        try:
            mcu_socket.sendall(data.encode('utf-8'))
        except Exception as e:
            print(f'Error sending data to Pico: {e}')
    return

@socketio.on('pico_connection')
def mcu_connection_handler():
    global mcu_socket
    global connected
    global save_images
    
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

            if save_images:
                # Velocities will be sent with a leading "_" 
                if full_data[0] == '_':
                    try:
                        if cap:
                            record_frame(cap.read())                            
                        command_array.append(full_data[1:])
                    except Exception as e:
                        print("error saving frame: {e}".format())
            print(full_data)

def capture_frames():
    current_time = time.time()
    while True:
        try:
            ret, frame = cap.read()

            ret, buffer = cv2.imencode('.jpg', frame)
            frame_buffer = buffer.tobytes()

            yield (b'--frame\r\n'
                    b'Content-Type: image/jpeg\r\n\r\n' + frame_buffer + b'\r\n')  # concat frame one by one and show result
            
            # trigger_record(ret, frame)
        except Exception as e:
            print("Error captutring frames: {e}".format())
            continue
        

@app.route('/video_feed')
def video_feed():
    if not args.cam:
        return
    global save_images
    return Response(capture_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

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
    print(data['data'])
    if data['data'] == "start":
        print('starting to record')
        global save_images
        save_images = True

        if connected:
            send_data('strt\n')
        else:
            print("Device not connected, stop and try again")


    elif data['data'] == "stop":
        print('ending recording')
        record_commands(command_array)
        save_images = False

        if connected:
            send_data('stpt\n')
##save_images variable not being updated correctly within video capture method. fix later
def trigger_record(ret, frame, vel):
    global command_array
    global current_time 
    # if save_images and (time.time() - current_time >= record_interval):
    record_frame(ret, frame)
    command_array.append(vel[1:].decode('utf-8'))

    current_time = time.time()


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
    
   