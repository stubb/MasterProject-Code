#include "WebSocketServer.h"
#include <getopt.h>
#include <libwebsockets.h>

WebSocketServer::WebSocketServer(){
	init();
}

WebSocketServer::WebSocketServer(int port){
	init();
	info.port = port;
}

void WebSocketServer::init() {
	memset(&info, 0, sizeof info);
	info.port = server_port;
	info.gid = -1;
	info.uid = -1;
	info.protocols = protocols;
}