var WebRTCConnection = function () {
    this.request = null;
    this.hangingGet = null;
    this.localName;
    this.server;
    this.myId = -1;
    this.otherPeers = {};
    this.messageCounter = 0;
    this.pc;    
    this.pcConfig = {"iceServers": [{"url": "stun:stun.l.google.com:19302"}]};
    this.pcOptions = {
        optional: [
            {DtlsSrtpKeyAgreement: true}
        ]
    }
    this.mediaConstraints = {'mandatory': {
        'OfferToReceiveAudio': true,
        'OfferToReceiveVideo': true }};
    this.remoteStream;
    
    RTCPeerConnection = window.mozRTCPeerConnection || window.webkitRTCPeerConnection;
    RTCSessionDescription = window.mozRTCSessionDescription || window.RTCSessionDescription;
    RTCIceCandidate = window.mozRTCIceCandidate || window.RTCIceCandidate;
    URL = window.webkitURL || window.URL;
}

WebRTCConnection.prototype.createPeerConnection = function(peer_id) {
        try {
            this.pc = new RTCPeerConnection(this.pcConfig, this.pcOptions);
            this.pc.onicecandidate = function(event) {
                if (event.candidate) {
                    var candidate = {
                        sdpMLineIndex: event.candidate.sdpMLineIndex,
                        sdpMid: event.candidate.sdpMid,
                        candidate: event.candidate.candidate
                    };
                    // send the offer SDP to remote peer
                    this.sendToPeer(peer_id, JSON.stringify(candidate));
                } else {
                  console.log("End of candidates.");
                }
            };
            var obj = this;
            this.pc.onconnecting = function(){obj.onSessionConnecting()};
            this.pc.onopen = function(){obj.onSessionOpened()};
            this.pc.onaddstream = function(){obj.onRemoteStreamAdded()};
            this.pc.onremovestream = function(){obj.onRemoteStreamRemoved()};
            console.log("Created RTCPeerConnnection with config: " + JSON.stringify(this.pcConfig));
        } 
        catch (e) {
            console.log("Failed to create PeerConnection with exception: " + e.message);
        }
    }
    
// Remote stream should be displayed.
WebRTCConnection.prototype.onRemoteStreamAdded = function(event) {
      console.log("Remote stream added:", URL.createObjectURL(event.stream));
     /* var remoteVideoElement = document.getElementById('remote-video');
      remoteVideoElement.src = URL.createObjectURL(event.stream);
      remoteVideoElement.play();*/
    }
    
WebRTCConnection.prototype.handlePeerMessage = function(peer_id, data) {
        ++this.messageCounter;
        var str = "Message from '" + this.otherPeers[peer_id] + "'&nbsp;";
        str += "<span id='toggle_" + this.messageCounter + "' onclick='toggleMe(this);' ";
        str += "style='cursor: pointer'>+</span><br>";
        str += "<blockquote id='msg_" + this.messageCounter + "' style='display:none'>";
        str += data + "</blockquote>";
        console.log(str);
        
        var dataJson = JSON.parse(data);
        console.log("received ", dataJson);
        if (data.search("offer") != -1) {
            this.createPeerConnection(peer_id);
            var obj = this;
            this.pc.setRemoteDescription(new RTCSessionDescription(dataJson), function(){obj.onRemoteSdpSucces(event)}, function(){obj.onRemoteSdpError()});              
            this.pc.createAnswer(function(sessionDescription) {
                console.log("Create answer:", sessionDescription);
                this.pc.setLocalDescription(sessionDescription);
                var data = JSON.stringify(sessionDescription);
                this.sendToPeer(peer_id, data);
            }, function(error) { // error
                console.log("Create answer error:", error);
            }, this.mediaConstraints); // type error  ); //}, null          
        }
        else {
            console.log("Adding ICE candiate ", dataJson);
            var candidate = new RTCIceCandidate({sdpMLineIndex: dataJson.sdpMLineIndex, candidate: dataJson.candidate});
            pc.addIceCandidate(candidate);
        }
    }    
     
WebRTCConnection.prototype.handleServerNotification = function(data) {
        console.log("Server notification: " + data);
        var parsed = data.split(',');
        if (this.parseInt(parsed[2]) != 0)
            this.otherPeers[this.parseInt(parsed[1])] = parsed[0];
    }
    
WebRTCConnection.prototype.parseIntHeader = function(r, name) {
        var val = r.getResponseHeader(name);
        return val != null && val.length ? parseInt(val) : -1;
    }
    
WebRTCConnection.prototype.hangingGetCallback = function() {
        try {
            if (this.hangingGet.readyState != 4)
                return;
            if (this.hangingGet.status != 200) {
                console.log("server error: " + this.hangingGet.statusText);
                this.disconnect();
            } else {
                var peer_id = this.parseIntHeader(this.hangingGet, "Pragma");
                console.log("Message from:", peer_id, ':', this.hangingGet.responseText);
                if (peer_id == this.myId) {
                  this.handleServerNotification(this.hangingGet.responseText);
                } else {
                  this.handlePeerMessage(peer_id, this.hangingGet.responseText);
                }
            }

            if (this.hangingGet) {
                this.hangingGet.abort();
                this.hangingGet = null;
            }

            if (this.myId != -1)
                window.setTimeout(this.startHangingGet, 0);
      } catch (e) {
          console.log("Hanging get error: " + e.description);
      }
    }
    
WebRTCConnection.prototype.startHangingGet = function() {
        try {
            this.hangingGet = new XMLHttpRequest();
			var obj = this;
            this.hangingGet.onreadystatechange = this.hangingGetCallback();
            this.hangingGet.ontimeout = function(){obj.onHangingGetTimeout()};
            this.hangingGet.open("GET", this.server + "/wait?peer_id=" + this.myId, true);
            this.hangingGet.send();  
        } catch (e) {
            console.log("error: " + e.description);
        }
    }
    
WebRTCConnection.prototype.onHangingGetTimeout = function() {
        console.log("hanging get timeout. issuing again.");
        this.hangingGet.abort();
        this.hangingGet = null;
        if (this.myId != -1)
            window.setTimeout(this.startHangingGet, 0);
    }
    
WebRTCConnection.prototype.signInCallback = function() {
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

WebRTCConnection.prototype.signIn = function(server, localName) {
    this.server = server;
    this.localName = localName;
      try {
          this.request = new XMLHttpRequest();
          var obj = this;
          this.request.onreadystatechange = function(){obj.signInCallback()};
          this.request.open("GET", this.server + "/sign_in?" + this.localName, true);
          this.request.send();
          console.log("request send: " + this.server + ", " + this.localName+ ", " + this.request.readyState)
      } catch (e) {
          console.log("error: " + e.description);
      }
    }
    
WebRTCConnection.prototype.sendToPeer = function(peer_id, data) {
      try {
          console.log(peer_id," Send ", data);
          if (this.myId == -1) {
              alert("Not connected");
              return;
          }
          if (peer_id == this.myId) {
              alert("Can't send a message to oneself :)");
              return;
          }
          var r = new XMLHttpRequest();
          r.open("POST", this.server + "/message?peer_id=" + this.myId + "&to=" + peer_id, false);
          r.setRequestHeader("Content-Type", "text/plain");
          r.send(data);
          console.log(peer_id," Send ", data);
          var dataJson = JSON.parse(data);
          console.log(peer_id," send ", data);
          r = null;
      } catch (e) {
          console.log("send to peer error: " + e.description);
      }
    }
   
/* GUI STUFF
WebRTCConnection.prototype.connect = function() {
        localName = document.getElementById("local").value.toLowerCase();
        server = document.getElementById("server").value.toLowerCase();
        if (localName.length == 0) {
            alert("I need a name please.");
            document.getElementById("local").focus();
        } else {
            document.getElementById("connect").disabled = true;
            document.getElementById("disconnect").disabled = false;
            //document.getElementById("send").disabled = false;
            signIn();
        }
    }
*/
    
WebRTCConnection.prototype.disconnect = function() {
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
      /* GUI stuff
        document.getElementById("connect").disabled = false;
        document.getElementById("disconnect").disabled = true;
        document.getElementById("send").disabled = true;
				*/
    }
    var obj = this;
    window.onbeforeunload = function(){obj.this.disconnect()};

/* GUI stuff
WebRTCConnection.prototype.send = function() {
        var text = document.getElementById("message").value;
        var peer_id = parseInt(document.getElementById("peer_id").value);
        if (!text.length || peer_id == 0) {
            alert("No text supplied or invalid peer id");
        } else {
            sendToPeer(peer_id, text);
        }
    }
*/

/* GUI stuff
WebRTCConnection.prototype.toggleMe = function(obj) {
        var id = obj.id.replace("toggle", "msg");
        var t = document.getElementById(id);
        if (obj.innerText == "+") {
            obj.innerText = "-";
            t.style.display = "block";
        } else {
            obj.innerText = "+";
            t.style.display = "none";
        }
    }
*/

WebRTCConnection.prototype.onSessionConnecting = function(message) {
	console.log("Session connecting.");
}
    
WebRTCConnection.prototype.onSessionOpened = function(message) {
	console.log("Session opened.");
}
    
 WebRTCConnection.prototype.onRemoteStreamRemoved = function(event) {
	console.log("Remote stream removed.");
}
    
WebRTCConnection.prototype.onRemoteSdpError = function(event) {
	console.error('onRemoteSdpError');
}
    
WebRTCConnection.prototype.onRemoteSdpSucces = function() {
	console.log('onRemoteSdpSucces');
}