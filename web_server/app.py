from flask import Flask, request, render_template, send_from_directory, Response
from flask_socketio import SocketIO
import socket
import threading
import cv2
import time

import sys, os
sys.path.append(os.path.abspath(os.path.join('..', 'AUTONOMOUS-DRIVING-PROJECT')))


from path_tracking.stream_processing import record      

app = Flask(__name__, template_folder=  'templates', static_folder= 'static_files')
socketio = SocketIO(app)
HOST = "0.0.0.0"
PORT = 5000

connected = False
save_images = False
record_interval = 10
stream_url = 'http://192.168.1.164/stream'  #home
cap = cv2.VideoCapture(stream_url)

current_time = time.time()


def send_data(data):
    if 'pico_socket' in globals():
        try:
            pico_socket.sendall(data.encode('utf-8'))
        except Exception as e:
            print(f'Error sending data to Pico: {e}')
    return

def pico_connection_handler():
    global pico_socket
    global connected
    
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.bind((HOST, PORT))
        server_socket.listen()

        print(f"Waiting for Pico Client to connect on {HOST}:{PORT}")
        pico_socket, addr = server_socket.accept()
        connected = True
        print('pico connected')
        while True:
            data = pico_socket.recv(1024)
            if not data:
                break
            print(data.decode('utf-8'))

def capture_frames():
    current_time = time.time()
    while True:

        ret, frame = cap.read()

        ret, buffer = cv2.imencode('.jpg', frame)
        frame_buffer = buffer.tobytes()

        yield (b'--frame\r\n'
                b'Content-Type: image/jpeg\r\n\r\n' + frame_buffer + b'\r\n')  # concat frame one by one and show result
        
        trigger_record(ret, frame)
        

@app.route('/video_feed')
def video_feed():
    global save_images
    return Response(capture_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

@app.route('/')
def controller():
    return render_template("index.html")

@socketio.on('button_click')
def button_click(data):
    print(data['data'])
    if connected:
        send_data(data['data'])


@socketio.on('start_record')
def start_record():
    print('starting to record')
    global save_images
    save_images = True

##save_images variable not being updated correctly within video capture method. fix later
def trigger_record(ret, frame):
    global current_time 
    if save_images and (time.time() - current_time >= record_interval):
        record(ret, frame)
        current_time = time.time()


@socketio.on('joystick_move')
def joystick_move(data):
    print(data['data'])
    if connected:
        send_data(data['data'])

@app.route('/js/index.js')
def serve_js(filename):
    return send_from_directory('static_files/js', filename)

@socketio.on('start_pico_thread')
def start_pico_thread():
    print('starting connection')
    pico_thread = threading.Thread(target=pico_connection_handler)
    pico_thread.start()


if __name__ == '__main__':

    socketio.run(app, host= '0.0.0.0', port= 8080, debug = True)
    
   