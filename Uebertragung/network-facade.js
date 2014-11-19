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

var supportedProtocols = ['WebRTC', 'WebSocket']

var NetworkFacade = function (protocol) {
	this.protocol = protocol;
	this.connection = null;
  console.log('NetworkFacade instance created with type ' + this.protocol);
	if(!supportedProtocols.contains(this.protocol)) {
  	console.log('NetworkFacade does not support protocol type ' + this.protocol);
	}
};

NetworkFacade.prototype.init = function() {
	if(this.protocol == "WebRTC") {
		console.log('NetworkFacade init called for protocol WebRTC');
		this.connection = new WebRTCConnection();
	}
	else if(this.protocol == "WebSocket") {
	}
	else {
		// do nothing
	}
};

NetworkFacade.prototype.connect = function(server, localName) {
	if(this.protocol == "WebRTC") {
		console.log('NetworkFacade connect called for protocol WebRTC');
		this.connection.signIn(server, localName);
	}
	else if(this.protocol == "WebSocket") {
	}
	else {
		// do nothing
	}
};

NetworkFacade.prototype.send = function() {
	if(this.protocol == "WebRTC") {
	}
	else if(this.protocol == "WebSocket") {
	}
	else {
		// do nothing
	}
};


NetworkFacade.prototype.disconnect = function() {
	if(this.protocol == "WebRTC") {
		console.log('NetworkFacade disconnect called for protocol WebRTC');
		this.connection.disconnect();
	}
	else if(this.protocol == "WebSocket") {
	}
	else {
		// do nothing
	}
};

// Stream
