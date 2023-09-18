var socket = io.connect('http://' + document.domain + ':'+location.port);


document.getElementById("forward").addEventListener("click", function(){
    socket.emit('button_click', {data: 'frwd'});
});
document.getElementById("left").addEventListener("click", function(){
    socket.emit('button_click', {data: 'lft'});
});
document.getElementById("right").addEventListener("click", function(){
    socket.emit('button_click', {data: 'rght'});
});
document.getElementById("backward").addEventListener("click", function(){
    socket.emit('button_click', {data: 'bck'});
});