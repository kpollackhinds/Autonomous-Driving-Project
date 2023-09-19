//******Reminder to change document.domain to location.hostname *****
var socket = io.connect('http://' + document.domain + ':'+location.port);


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
