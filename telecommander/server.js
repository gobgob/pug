var Com = require('./communication');

var com = new Com();

var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);

com.on('ready', function(){
	
	app.get('/', function(req, res){
		res.sendfile('client.html');
	});
	
	io.on('connection', function(socket){
		console.log("hello client");
		
		socket.on('leftMotor', function(s){
			var speed = ("000" + Maths.abs(s)).substr(-3); 
			if (s > 0){
				com.write( "L" + speed );
			}else{
				com.write( "l" + speed );
			}
		});

		socket.on('rightMotor', function(s){
			var speed = ("000" + Maths.abs(s)).substr(-3); 
			if (s > 0){
				com.write( "R" + speed );
			}else{
				com.write("r" + speed);
			}
		});
	});

	
	http.listen(80, function (){
		console.log("oh yeah!");
	})

});
