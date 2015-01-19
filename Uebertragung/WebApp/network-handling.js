/**
 * Negotiate the network connection with the PeerManager
 */

var NetworkHandlingClient = function () {
	// XMLHttpRequest
	this.request = null;
	
    // XMLHttpRequest
    this.hangingGet = null;
	
    this.localName;
    this.server;
    this.myId = -1;
    this.otherPeers = {};
	this.connection = null;
}

NetworkHandlingClient.prototype.isConnected = function() {
	if (this.myId == -1) {
		return false;
	} else {
		return true;
	}
} 
     
NetworkHandlingClient.prototype.handleServerNotification = function(data) {
        console.log("Server notification: " + data);
        var parsed = data.split(',');
        if (this.parseInt(parsed[2]) != 0)
            this.otherPeers[this.parseInt(parsed[1])] = parsed[0];
    }
    
NetworkHandlingClient.prototype.parseIntHeader = function(r, name) {
        var val = r.getResponseHeader(name);
        return val != null && val.length ? parseInt(val) : -1;
    }
    
NetworkHandlingClient.prototype.hangingGetCallback = function() {
        try {
            if (this.hangingGet.readyState != 4)
                return;
            if (this.hangingGet.status != 200) {
                console.log("server error: " + this.hangingGet.statusText);
                this.disconnect();
            } else {
                var peer_id = this.parseIntHeader(this.hangingGet, "Pragma");
				console.log("peer_id: " + peer_id);
                console.log("Message from:", peer_id, ':', this.hangingGet.responseText);
                if (peer_id == this.myId) {
                  this.handleServerNotification(this.hangingGet.responseText);
                } else {
                  this.connection.handlePeerMessage(peer_id, this.hangingGet.responseText);
                }
            }

            if (this.hangingGet) {
                this.hangingGet.abort();
                this.hangingGet = null;
            }

           // if (this.myId != -1)
            //    window.setTimeout(this.startHangingGet, 0);
            this.startHangingGet();
      } catch (e) {
          console.log("Hanging get error: " + e.description);
      }
   // }
}

NetworkHandlingClient.prototype.startHangingGet = function() {
	try {
		var obj = this;
		obj.hangingGet = new XMLHttpRequest();
		obj.hangingGet.onreadystatechange = function(){obj.hangingGetCallback()};
		obj.hangingGet.ontimeout = function(){obj.onHangingGetTimeout()};
		obj.hangingGet.open("GET", obj.server + "/wait?peer_id=" + obj.myId, true);
		obj.hangingGet.send();
	} catch (e) {
		console.log("error: " + e.description);
	}
}
    
NetworkHandlingClient.prototype.onHangingGetTimeout = function() {
        console.log("hanging get timeout. issuing again.");
        this.hangingGet.abort();
        this.hangingGet = null;
		this.startHangingGet();
    }
    
NetworkHandlingClient.prototype.signInCallback = function() {
        try {
            if (this.request.readyState == 4) {
                if (this.request.status == 200) {
                    var peers = this.request.responseText.split("\n");
                    this.myId = parseInt(peers[0].split(',')[1]);
                    console.log("My id: " + this.myId);
                    for (var i = 1; i < peers.length; ++i) {
                        if (peers[i].length > 0) {
                            console.log("Peer " + i + ": " + peers[i]);
                            var parsed = peers[i].split(',');
                            this.otherPeers[parseInt(parsed[1])] = parsed[0];
                        }
                    }
                    this.startHangingGet();
                    this.request = null;
                }
            }
        } catch (e) {
            console.log("error: " + e.description);
        }
    }

NetworkHandlingClient.prototype.signIn = function(server, localName, connection) {
    this.server = server;
    this.localName = localName;
	this.connection = connection;
	console.log("conection " + connection.test);
      try {
			var obj = this;
          obj.request = new XMLHttpRequest();
          obj.request.onreadystatechange = function(){obj.signInCallback()};
          obj.request.open("GET", obj.server + "/sign_in?" + obj.localName, true);
          obj.request.send();
          console.log("request send: " + obj.server + ", " + obj.localName+ ", " + obj.request.readyState)
      } catch (e) {
          console.log("error: " + e.description);
      }
    }
       
NetworkHandlingClient.prototype.disconnect = function() {
        if (this.request) {
            this.request.abort();
            this.request = null;
        }
        
        if (this.hangingGet) {
            this.hangingGet.abort();
            this.hangingGet = null;
        }
      
        if (this.myId != -1) {
            this.request = new XMLHttpRequest();
            this.request.open("GET", this.server + "/sign_out?peer_id=" + this.myId, false);
            this.request.send();
            this.request = null;
            this.myId = -1;
        }
    var obj = this;
    window.onbeforeunload = function(){obj.this.disconnect()};
}