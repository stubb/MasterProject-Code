// extends Array class with contains method
Array.prototype.contains = function(obj) {
    var i = this.length;
    while (i--) {
        if (this[i] === obj) {
            return true;
        }
    }
    return false;
}

var supportedProtocols = ['WebRTC', 'WebSocket'];
var video_source = null;

var NetworkFacade = function (protocol) {
	this.protocol = protocol;
	// NOTE: only used by WebSocket right now!
	this.connection = null;
	this.remoteUrl = null;
	this.localUserName = null;
	console.log('NetworkFacade instance created with type ' + this.protocol);
	if(!supportedProtocols.contains(this.protocol)) {
		console.log('NetworkFacade does not support protocol type ' + this.protocol);
	}
	this.streamingVideo = null;
};

NetworkFacade.prototype.init = function(server, localName) {
	console.log("NetworkFacad Init called");
	this.remoteUrl = url = server;
	this.localUserName = user = localName;
	
	if(this.protocol == "WebRTC") {
		connectFromHere(this.remoteUrl, this.localUserName);
	}
	else if (this.protocol == "WebSocket") {
		// NOTE: hardcoded sub-protocol for websockets
		this.connection = new WebSocket(this.remoteUrl, 'callback_save_data');
		
		this.connection.onopen = function() {
			console.log("Success, connected to " + url);
		};
			
		this.connection.onerror = function() {
			console.log("Error, cant connect to " + url);
		};
		
		this.connection.onclose = function () {};
	}
	else {
		// do nothing
		console.log('Error');
	}
}

NetworkFacade.prototype.disconnect = function() {
	if(this.protocol == "WebRTC") {
		console.log('Not implemented yet!');
	}
	else if(this.protocol == "WebSocket") {
		this.connection.close();
	}
	else {
		// do nothing
		console.log('Error');
	}
};

NetworkFacade.prototype.startStreamVideo = function(video_source, width) {
	if(this.protocol == "WebRTC") {
		console.log('NetworkFacade init called for protocol WebRTC');
		addLocalStreamFromHere();
		negotiateCallFromHere();
		return true;
	}
	else if(this.protocol == "WebSocket") {
		var canvas = document.createElement("canvas");
		canvas.setAttribute("type", "hidden");
		var nwf = this;
		this.streamingVideo = setInterval(function(){
			drawPictureToCanvas(video_source, video_source.videoWidth, video_source.videoHeight, canvas, width);
			_image_data = canvas.toDataURL("image/jpeg");
			_image_data = _image_data.replace(/^data:image\/jpeg;base64,/ig, "");
			nwf.sendData(getXMLforPicture(canvas.width, canvas.height, _image_data));
		}, 50);
		return true;
	}
	else {
		// do nothing
		console.log('Error');
		return false;
	}
};

NetworkFacade.prototype.stopStreamVideo = function(e) {
	if(this.protocol == "WebRTC") {
		hangUpFromHere();
	}
	else if(this.protocol == "WebSocket") {
		clearInterval(this.streamingVideo);
	}
	else {
		// do nothing
		console.log('Error');
	}
};

NetworkFacade.prototype.sendData = function(data) {
	if(this.protocol == "WebRTC") {
		console.log('NetworkFacade init called for protocol WebRTC');
		createDataChannelFromHere();
		sendDataFromHere(data);
	}
	else if(this.protocol == "WebSocket") {
		if (this.connection.readyState == 1) {
			this.connection.send(data);
			return 0;
		}
		else {
			return "Connection not open";
		}
	}
	else {
		// do nothing
		console.log('Error');
	}
};