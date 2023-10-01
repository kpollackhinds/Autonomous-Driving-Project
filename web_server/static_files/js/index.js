import JoyStick from "./joy.js";

//******Reminder to change document.domain to location.hostname *****
var socket = io.connect('http://' + document.domain + ':'+location.port);

var cliked_count = 0;
const toggleJoystick = document.getElementById("toggle-joystick");

const forwardButton = document.getElementById("forward");
const leftButton = document.getElementById("left");
const rightButton = document.getElementById("right");
const backButton = document.getElementById("backward");
const startButton = document.getElementById("start-button");

const directionalButtons = document.getElementById("direction-buttons");
const joystick = document.getElementById("joyDiv");



// Event Listeners for when button is clicked/being clicked

forwardButton.addEventListener("mousedown", function(){
    socket.emit('button_click', {data: 'frwd'});
});

leftButton.addEventListener("mousedown", function(){
    socket.emit('button_click', {data: 'lft'});
});
rightButton.addEventListener("mousedown", function(){
    socket.emit('button_click', {data: 'rght'});
});
backButton.addEventListener("mousedown", function(){
    socket.emit('button_click', {data: 'bck'});
});

startButton.addEventListener("click", function(){
    socket.emit('start_pico_thread');
});

// Event listener for when button is released
forwardButton.addEventListener("mouseup", function(){
    socket.emit('button_click', {data: 'stp'});
});
leftButton.addEventListener("mouseup", function(){
    socket.emit('button_click', {data: 'stp'});
});
rightButton.addEventListener("mouseup", function(){
    socket.emit('button_click', {data: 'stp'});
});
backButton.addEventListener("mouseup", function(){
    socket.emit('button_click', {data: 'stp'});
});



//Switching between joystick and directional controls
toggleJoystick.addEventListener('change', function(){
    if (toggleJoystick.checked){
        joystick.style.display = 'block';
        directionalButtons.style.display = 'none';

        if (cliked_count == 0){
            cliked_count = 1;
            function sendData(){
                var prev = [0, 0];
                
                
                var x = joy.GetX();
                var y = joy.GetY();
                
                if (x > 100){x = 100;}
                else if (x < -100){x = -100}

                if (y > 100){y = 100;}
                else if (y < -100){y = -100;};
                
                if (prev[0] != x || prev[1] != y){
                    var data_s = String(x) + ',' + String(y)

                    socket.emit('joystick_move', {data: data_s});
                    console.log(joy.GetX(), joy.GetY());

                    prev[0] = x;
                    prev[1] = y;
                };
            }

            var joy = new JoyStick('joyDiv', {}, sendData);
        }

    }
    else {
        directionalButtons.style.display = 'block';
        joystick.style.display = 'none'
    }
});

