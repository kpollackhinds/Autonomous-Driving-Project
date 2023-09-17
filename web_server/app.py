from flask import Flask, jsonify, request, render_template, send_from_directory
from flask_socketio import SocketIO

app = Flask(__name__, template_folder=  'templates', static_folder= 'static_files')
socketio = SocketIO(app)

@app.route('/')
def controller():
    return render_template("index.html")

@socketio.on('button_click')
def button_click(data):
    print(data['data'])

@app.route('/js/index.js')
def serve_js(filename):
    return send_from_directory('static_files/js', filename)

if __name__ == '__main__':
    socketio.run(app, host= '0.0.0.0', port= 5000)