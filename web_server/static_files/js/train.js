
//******Reminder to change document.domain to location.hostname *****
var socket = io.connect('http://' + document.domain + ':'+location.port);

const recordButton = document.getElementById("record-button");
let recordState = 'initial';


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