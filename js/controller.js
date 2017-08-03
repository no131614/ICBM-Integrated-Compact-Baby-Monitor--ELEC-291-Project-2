var socket = io();

function up(){
	socket.emit('up');
}
function down(){
	socket.emit('down');
}
function record(){
	socket.emit('record');
}
function play(){
	socket.emit('play');
}