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
const recordButton = document.getElementById("record-button");


const directionalButtons = document.getElementById("direction-buttons");
const joystick = document.getElementById("joyDiv");

var prev = ["1", 0, 0];
var interval = 5;
var counter = 0;

let connectionState = 'initial';
let recordState = 'initial';


// Event Listeners for when button is clicked/being clicked

forwardButton.addEventListener("mousedown", function(){
    socket.emit('button_click', {data: 'frwd\n'});
});

leftButton.addEventListener("mousedown", function(){
    socket.emit('button_click', {data: 'lft\n'});
});
rightButton.addEventListener("mousedown", function(){
    socket.emit('button_click', {data: 'rght\n'});
});
backButton.addEventListener("mousedown", function(){
    socket.emit('button_click', {data: 'bck\n'});
});


startButton.addEventListener("click", function(){
    socket.emit('start_mcu_thread');

    switch (connectionState){
        case 'initial':
            connectionState = 'connecting';
            startButton.textContent = 'Connecting...';
            
            startButton.classList.remove('start-button');
            startButton.classList.add('connecting-button');
            break;
        
        case 'connecting':
            // add additional error handling or functionality later
            break;

        case 'connected':
            // add additional functionality later 
            break
    }
});

recordButton.addEventListener("click", function(){

    switch(recordState){
        
        case 'initial':
            socket.emit('manage_record', {data: "start"});
            recordState = 'Recording';
            recordButton.textContent = 'Stop Recording';

            recordButton.classList.remove('record-button');
            recordButton.classList.add('recording-button');

            break;
        
        case 'Recording':
            socket.emit('manage_record', {data:'stop'});
            recordState = 'initial';
            recordButton.textContent = 'Start Recording';

            recordButton.classList.remove('recording-button');
            recordButton.classList.add('record-button');

            break;
    }

})

socket.on('mcu_connected', function(){
    connectionState = 'connected';
    startButton.textContent = 'Connected';

    startButton.classList.remove('connecting-button');
    startButton.classList.add('connected-button');
})



// Event listener for when button is released
forwardButton.addEventListener("mouseup", function(){
    socket.emit('button_click', {data: 'stp\n'});
});
leftButton.addEventListener("mouseup", function(){
    socket.emit('button_click', {data: 'stp\n'});
});
rightButton.addEventListener("mouseup", function(){
    socket.emit('button_click', {data: 'stp\n'});
});
backButton.addEventListener("mouseup", function(){
    socket.emit('button_click', {data: 'stp\n'});
});



//Switching between joystick and directional controls
toggleJoystick.addEventListener('change', function(){
    if (toggleJoystick.checked){
        joystick.style.display = 'block';
        directionalButtons.style.display = 'none';

        if (cliked_count == 0){
            cliked_count = 1;
            function sendData(){
                // if (counter % interval != 0){
                //     counter +=1;
                //     return
                // }               
                var x = joy.GetX();
                var y = joy.GetY();
          
                var theta = Math.atan2(y, x);
                var magnitude = Math.floor(Math.sqrt((x*x) + (y*y)));

                //technically end ranges of joystick make a square shape instead of circle >:( . Quick fix to make math easier 
                if (magnitude > 100){
                    magnitude = 100
                };

                if (theta >= 0){
                    var dir = "1";
                    //speeds if in first quadrant
                    if (x >= 0){
                        var left_speed = magnitude
                        var right_speed = Math.floor(100 * theta/(Math.PI/2)) 
                    }
                    //speeds if in second quadrant
                    else{
                        var left_speed = Math.floor(100 * (Math.PI -theta)/(Math.PI/2))
                        var right_speed = magnitude
                    }
                }
                else{
                    var dir = "-1";
                    //speeds if in fourth quadrant
                    if(x >= 0){
                        var left_speed = magnitude
                        var right_speed = Math.floor(-100 * theta/(Math.PI/2))
                    }
                    //speeds if in third quadrant
                    else{
                        var left_speed = Math.floor(-100 * (Math.PI + theta)/(Math.PI/2))
                        var right_speed = magnitude
                    }
                };

                //avoiding sending repeated values bc not necessary
                if (prev[0] != dir || prev[1] != x || prev[2] != y){
                    var data_s = dir + ',' + String(Math.abs(left_speed)) + ',' + String(Math.abs(right_speed)) + '\n';

                    socket.emit('joystick_move', {data: data_s});
                    console.log(x, y);
                    console.log(left_speed, right_speed);

                    prev[0] = dir;
                    prev[1] = x;
                    prev[2] = y;

                    counter +=1;
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

