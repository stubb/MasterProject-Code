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
	this.handler = null;
	this.connection = null;
	console.log('NetworkFacade instance created with type ' + this.protocol);
	if(!supportedProtocols.contains(this.protocol)) {
	console.log('NetworkFacade does not support protocol type ' + this.protocol);
	}
};

NetworkFacade.prototype.init = function() {
	this.handler = new NetworkHandlingClient();
	if(this.protocol == "WebRTC") {
		console.log('NetworkFacade init called for protocol WebRTC');
		this.connection = new WebRTCConnection();
	}
	else if(this.protocol == "WebSocket") {
	}
	else {
		// do nothing
		console.log('Error');
	}
};

/** Sign in at the peerserver to be able to communicate with other peers.
 * Will get the response from one assigned peer **/
NetworkFacade.prototype.connect = function(server, localName) {
	console.log(this.connection == null);
	this.handler.signIn(server, localName, this.connection);
};

NetworkFacade.prototype.streamVideo = function(source) {
	if(this.protocol == "WebRTC") {
		//this.connection
	}
	else if(this.protocol == "WebSocket") {
	}
	else {
		// do nothing
	}
};


NetworkFacade.prototype.disconnect = function() {
	this.handler.disconnect();
};

// Stream
