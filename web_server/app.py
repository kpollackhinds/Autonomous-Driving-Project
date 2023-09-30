from flask import Flask, jsonify, request, render_template, send_from_directory
from flask_socketio import SocketIO
import socket
import threading

app = Flask(__name__, template_folder=  'templates', static_folder= 'static_files')
socketio = SocketIO(app)
HOST = "0.0.0.0"
PORT = 5000

connected = False

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

@app.route('/')
def controller():
    return render_template("index.html")

@socketio.on('button_click')
def button_click(data):
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
    # pico_thread = threading.Thread(target=pico_connection_handler)
    # pico_thread.start() 
    socketio.run(app, host= '0.0.0.0', port= 8080, debug = True)
    
   