#include <getopt.h>
#include <libwebsockets.h>
#include <iostream>

#include "WebSocketServer.h"

using namespace std;

unsigned int position = 0;

WebSocketServer::WebSocketServer(){
	init();
}

WebSocketServer::WebSocketServer(int port){
	init();
	info.port = port;
}

int WebSocketServer::init() {
	int return_code = 0;
	init_protocols();
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

void WebSocketServer::init_protocols() {
	protocols[0] = 
		{
			"http-only",   // name
			callback_http, // callback
			0              // per_session_data_size
		};
		
	protocols[1] = 		
		{
			"callback_save_data",
			callback_save_data,
			0,
			65536				// rx_buffer_size
		};
		
	protocols[2] = 
		{
			NULL, NULL, 0   /* End of list */
		};
}

int WebSocketServer::run() {
	return libwebsocket_service(context, 100);
}

void WebSocketServer::destroy() {
	libwebsocket_context_destroy(context);
}