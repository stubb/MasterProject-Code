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
	this.connection = null;
	console.log('NetworkFacade instance created with type ' + this.protocol);
	if(!supportedProtocols.contains(this.protocol)) {
	console.log('NetworkFacade does not support protocol type ' + this.protocol);
	}
};

NetworkFacade.prototype.startStreamVideo = function(server, localName, source) {
	if(this.protocol == "WebRTC") {
		console.log('NetworkFacade init called for protocol WebRTC');
		addLocalStreamFromHere();
		negotiateCallFromHere();
	}
	else if(this.protocol == "WebSocket") {
		console.log('Not implemented yet!');	
	}
	else {
		// do nothing
		console.log('Error');
	}
};

NetworkFacade.prototype.stopStreamVideo = function(e) {
	if(this.protocol == "WebRTC") {
		hangUpFromHere();
	}
	else if(this.protocol == "WebSocket") {
		console.log('Not implemented yet!');	
	}
	else {
		// do nothing
		console.log('Error');
	}
};