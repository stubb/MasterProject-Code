#include <getopt.h>
#include <libwebsockets.h>
#include <iostream>

#include "WebSocketServer.h"

using namespace std;

unsigned int position = 0;
char* recv_buffer = new char[104857600]; // 100 MB Buffer.

WebSocketServer::WebSocketServer(){
	init();
}

WebSocketServer::WebSocketServer(int port){
	init();
	info.port = port;
}

int WebSocketServer::init() {
	int return_code = 0;
	memset(&info, 0, sizeof info);
	info.port = server_port;
	info.gid = -1;
	info.uid = -1;
	info.protocols = protocols;
	context = libwebsocket_create_context(&info);
	if (context == NULL)
	{
		cerr << "libwebsocket init failed" << endl;
		return_code = EXIT_FAILURE;
	}
	return return_code;
}

int WebSocketServer::run() {
	return libwebsocket_service(context, 100);
}

void WebSocketServer::destroy() {
	libwebsocket_context_destroy(context);
}