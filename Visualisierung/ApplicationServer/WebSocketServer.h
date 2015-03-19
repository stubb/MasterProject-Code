#ifndef __WEBSOCKETSERVER_H__
#define __WEBSOCKETSERVER_H__

#define DEBUG 1

/*	Libwebsockets Includes.	*/
#include <getopt.h>
#include <libwebsockets.h>
#include "MonkeyMediaProcessor.h"
#include <iostream>

extern MonkeyMediaProcessor* mmp;

using namespace std;

class WebSocketServer {

	private:
		struct libwebsocket_context *context;
		struct lws_context_creation_info info;
		int return_code = 0;
		int server_port = 9000;
		int init();

	public:
		WebSocketServer();
		WebSocketServer(int port);
		int run();
		void destroy();
};

static int callback_http(	struct libwebsocket_context * that,
					struct libwebsocket *wsi,
				   enum libwebsocket_callback_reasons reason,
				   void *user,
				   void *in,
				   size_t len)
{
	return 0;
}

static int callback_save_data(	struct libwebsocket_context * that,
						struct libwebsocket *wsi,
						enum libwebsocket_callback_reasons reason,
						void *user,
						void *in,
						size_t len)
{
	unsigned int position = 0;
	char* recv_buffer = new char[104857600]; // 100 MB Buffer
	switch (reason)
	{
		case LWS_CALLBACK_ESTABLISHED:
			cout << "Connection established" << endl;
			break;
		case LWS_CALLBACK_RECEIVE: {
			memcpy(recv_buffer + position, in, len);
			position += len;
			const size_t remaining = libwebsockets_remaining_packet_payload(wsi);
			if (remaining == 0)
			{
				#if DEBUG
					cout << "Done receiving data. Got " << position << " Bytes." << endl;
				#endif
				
				char* xml_string = new char[position];
				memcpy(xml_string, recv_buffer, position);
				#if DEBUG
					for (int i = 0; i < 200; i++) {
						cout << xml_string[i];
					}
					cout << endl << endl;
				#endif
				mmp->process_monkey_data(xml_string, position, 1);
				position = 0; // Reset position.
			}
			break;
		}
		case LWS_CALLBACK_CLOSED: {
			cout << "Connection closed" << endl;
			break;
		}
		default:
			break;
	}
	
	return 0;
}

static struct libwebsocket_protocols protocols[] = {
	{
		"http-only",   // name
		callback_http, // callback
		0              // per_session_data_size
	},
	{
		"callback_save_data",
		callback_save_data,
		0,
		65536				// rx_buffer_size
	},
	{
		NULL, NULL, 0   /* End of list */
	}
};
#endif



