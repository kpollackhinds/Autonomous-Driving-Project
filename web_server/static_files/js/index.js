import JoyStick from "./joy.js";
//******Reminder to change document.domain to location.hostname *****
var socket = io.connect('http://' + document.domain + ':'+location.port);

// var joy = new JoyStick('joyDiv');
var cliked_count = 0;

// Event Listeners for when button is clicked/being clicked
document.getElementById("forward").addEventListener("mousedown", function(){
    socket.emit('button_click', {data: 'frwd'});
});
document.getElementById("left").addEventListener("mousedown", function(){
    socket.emit('button_click', {data: 'lft'});
});
document.getElementById("right").addEventListener("mousedown", function(){
    socket.emit('button_click', {data: 'rght'});
});
document.getElementById("backward").addEventListener("mousedown", function(){
    socket.emit('button_click', {data: 'bck'});
});

document.getElementById("start-button").addEventListener("click", function(){
    socket.emit('start_pico_thread');
});

// Event listener for when button is released
document.getElementById("forward").addEventListener("mouseup", function(){
    socket.emit('button_click', {data: 'stp'});
});
document.getElementById("left").addEventListener("mouseup", function(){
    socket.emit('button_click', {data: 'stp'});
});
document.getElementById("right").addEventListener("mouseup", function(){
    socket.emit('button_click', {data: 'stp'});
});
document.getElementById("backward").addEventListener("mouseup", function(){
    socket.emit('button_click', {data: 'stp'});
});


//Switching between joystick and directional controls
const toggleJoystick = document.getElementById("toggle-joystick");
const directionalButtons = document.getElementById("direction-buttons");
const joystick = document.getElementById("joyDiv");


toggleJoystick.addEventListener('change', function(){
    if (toggleJoystick.checked){
        joystick.style.display = 'block';
        directionalButtons.style.display = 'none';

        if (cliked_count == 0){
            cliked_count = 1;
            var joy = new JoyStick('joyDiv');
        }

    }
    else {
        directionalButtons.style.display = 'block';
        joystick.style.display = 'none'
    }
});

