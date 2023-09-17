var socket = io.connect('http://' + document.domain + ':'+location.port);


document.getElementById("forward").addEventListener("click", function(){
    socket.emit('button_click', {data: 'forward button clicked'});
});