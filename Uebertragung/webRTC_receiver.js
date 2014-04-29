// get a reference to our FireBase database. You should create your own
// and replace the URL.
var dbRef = new Firebase("https://amber-fire-9831.firebaseio.com");
var roomRef = dbRef.child("rooms");

// shims!
var PeerConnection = window.mozRTCPeerConnection || window.webkitRTCPeerConnection;
var SessionDescription = window.mozRTCSessionDescription || window.RTCSessionDescription;
var IceCandidate = window.mozRTCIceCandidate || window.RTCIceCandidate;
navigator.getUserMedia = navigator.getUserMedia || navigator.mozGetUserMedia || navigator.webkitGetUserMedia;

// generate a unique-ish string
function id () {
	return (Math.random() * 10000 + 10000 | 0).toString();
}

// a nice wrapper to send data to FireBase
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

// generate a unique-ish room number
var ME = id();
var ROOM = "xXx";

// get references to the document tags
var chatlog = document.getElementById("chatlog");

// options for the PeerConnection
var server = {
	iceServers: [
		{url: "stun:stun.l.google.com:19302"}
	]
};

var options = {
	optional: [
		{DtlsSrtpKeyAgreement: true},
		{RtpDataChannels: true} //required for Firefox
	]
}

// create the PeerConnection
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

// constraints on the offer SDP.
var constraints = {};

// define the channel var
var channel;

connect();

// start the connection!
function connect () {
		// answerer must wait for the data channel
		pc.ondatachannel = function (e) {
			channel = e.channel;
			bindEvents(); //now bind the events
		};

		// answerer needs to wait for an offer before
		// generating the answer SDP
		recv(ROOM, "offer", function (offer) {
			pc.setRemoteDescription(
				new SessionDescription(JSON.parse(offer))
			);

			// now we can generate our answer SDP
			pc.createAnswer(function (answer) {
				pc.setLocalDescription(answer);

				// send it to FireBase
				send(ROOM, "answer", JSON.stringify(answer));
			}, errorHandler, constraints);	
		});	
	}

var arrayToStoreChunks = [];

// bind the channel events
function bindEvents () {
	channel.onopen = function () { console.log("Channel Open"); }
	channel.onmessage = function (event) {

		console.log(event.data.toString());
		 var data = JSON.parse(event.data);

    arrayToStoreChunks.push(data.message); // pushing chunks in array

    if (data.last) {
        saveToDisk(arrayToStoreChunks.join(''), data.filename);
        arrayToStoreChunks = []; // resetting array
    }
	};
	channel.onclose = function () { console.log("Channel Close"); }
	var rooms = new Firebase('https://amber-fire-9831.firebaseio.com/rooms');
	rooms.remove();
}
function saveToDisk(fileUrl, fileName) { 
		console.log("saveToDisk"); 
    var s = document.getElementById('foo');
    s.style.display = "inline";
		s.href = fileUrl; 
    s.download = fileName || fileUrl; 
}