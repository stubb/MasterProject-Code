var PeerConnection = window.mozRTCPeerConnection || window.webkitRTCPeerConnection;
var IceCandidate = window.mozRTCIceCandidate || window.RTCIceCandidate;
var SessionDescription = window.mozRTCSessionDescription || window.RTCSessionDescription;
navigator.getUserMedia = navigator.getUserMedia || navigator.mozGetUserMedia || navigator.webkitGetUserMedia;

var dbRef = new Firebase("https://amber-fire-9831.firebaseio.com");
var roomRef = dbRef.child("rooms");
var chunkLength = 100000;
var file = null;

// generate a unique-ish string
function id () {
	return (Math.random() * 10000 + 10000 | 0).toString();
}

function send (room, key, data) {
	roomRef.child(room).child(key).set(data);
}

// wrapper function to receive data from FireBase
function recv (room, type, cb) {
	roomRef.child(room).child(type).on("value", function (snapshot, key) {
		var data = snapshot.val();
		if (data) { cb(data); }
	});
}

// generic error handler
function errorHandler (err) {
	console.error(err);
}

var ME = id();
var ROOM = "xXx";
var chatlog = document.getElementById("chatlog");
//var message = document.getElementById("message");

var server = {
    iceServers: [
        {url: "stun:stun.l.google.com:19302"}
    ]
}

var options = {
    optional: [
        {DtlsSrtpKeyAgreement: true},
        {RtpDataChannels: true}
    ]
}

var pc = new PeerConnection(server, options);

pc.onicecandidate = function (e) {
	
	// take the first candidate that isn't null
	if (!e.candidate) { return; }
	pc.onicecandidate = null;

	// request the other peers ICE candidate
	recv(ROOM, "candidate:" + otherType, function (candidate) {
		pc.addIceCandidate(new IceCandidate(JSON.parse(candidate)));
	});

	// send our ICE candidate
	send(ROOM, "candidate:"+type, JSON.stringify(e.candidate));
};

var channel;
var constraints = {};

connect();

function connect () {
	
	channel = pc.createDataChannel("mychannel", {});
	//channel.binaryType = "blob";

	// can bind events right away
	bindEvents();

	// create the offer SDP
	pc.createOffer(function (offer) {
	pc.setLocalDescription(offer);

	// send the offer SDP to FireBase
	send(ROOM, "offer", JSON.stringify(offer));

	// wait for an answer SDP from FireBase
	recv(ROOM, "answer", function (answer) {
		pc.setRemoteDescription(
			new SessionDescription(JSON.parse(answer))
		);
	});
	}, errorHandler, constraints);
}

function bindEvents () {
	channel.onopen = function () { console.log("Channel Open"); }
	channel.onmessage = function (event) {
		// add the message to the chat log
		chatlog.innerHTML += "<div>Peer says: " + event.data + "</div>";
	};
	channel.onclose = function () {
		console.log("Channel Close");
		var rooms = new Firebase('https://amber-fire-9831.firebaseio.com/rooms');
		rooms.remove();
	}
}

function sendMessage () {
	var reader = new window.FileReader();
	reader.readAsDataURL(file);
	reader.onload = onReadAsDataURL;
}

function onReadAsDataURL(event, text) {
    var data = {}; // data object to transmit over data channel

    if (event) text = event.target.result; // on first invocation

    if (text.length > chunkLength) {
        data.message = text.slice(0, chunkLength); // getting chunk using predefined chunk length
    } else {
        data.message = text;
        data.last = true;
				data.filename = file.name;
    }

    channel.send(JSON.stringify(data)); // use JSON.stringify for chrome!

    var remainingDataURL = text.slice(data.message.length);
    if (remainingDataURL.length) setTimeout(function () {
        onReadAsDataURL(null, remainingDataURL); // continue transmitting
    }, 10)
}

	document.querySelector('input[type=file]').onchange = function() {
  	file = this.files[0];
	};
