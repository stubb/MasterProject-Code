var WebRTCConnection = function () {
    this.pc;    
    this.pcConfig = {"iceServers": [{"url": "stun:stun.l.google.com:19302"}]};
    this.pcOptions = {
        optional: [
            {DtlsSrtpKeyAgreement: true}
        ]
    }
    this.mediaConstraints = {'mandatory': {
        'OfferToReceiveAudio': false,
        'OfferToReceiveVideo': true }};
    this.remoteStream;
	this.test = "hiho";
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
            console.log("Created RTCPeerConnnection with config: " + JSON.stringify(this.pcConfig) + JSON.stringify(this.pc));
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
        var dataJson = JSON.parse(data);
        console.log("received ", dataJson);
        if (data.search("offer") != -1) {
            this.createPeerConnection(peer_id);
            var obj = this;
            this.pc.setRemoteDescription(new RTCSessionDescription(dataJson), function(){this.onRemoteSdpSucces();this.pc.createAnswer(function(sessionDescription) {
                console.log("Create answer:", sessionDescription);
                this.pc.setLocalDescription(sessionDescription);
                var data = JSON.stringify(sessionDescription);
                this.sendToPeer(peer_id, data);
            }, function(error) { // error
                console.log("Create answer error:", error);
            }, this.mediaConstraints);}, null);
 // type error  ); //}, null          
        }
        else {
            console.log("Adding ICE candidate ", dataJson);
            var candidate = new RTCIceCandidate({sdpMLineIndex: dataJson.sdpMLineIndex, candidate: dataJson.candidate});
            this.pc.addIceCandidate(candidate,
									function(){console.log("addIceCandidate success")},
									function(error){console.log("addIceCandidate fail", JSON.stringify(error))});
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

