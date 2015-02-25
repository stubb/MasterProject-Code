#ifndef __AS_H__
#define __AS_H__

/*	Standard Includes.	*/
#include <iostream>

/*	OpenCV Includes.	*/
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

/*	SDL2 Includes.	*/
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

/*	Libwebsockets Includes.	*/
#include <getopt.h>
#include <libwebsockets.h>

using namespace cv;
using namespace std;

//	============================================================
//						GLOBAL VARIABLES
//	============================================================
MonkeyMediaProcessor *mmp = NULL;
unsigned int position = 0;

//	============================================================
//					LIBWEBSOCKET FUNCTIONS
//	============================================================

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
	char recv_buffer[104857600]; // 100 MB Buffer.
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
					cout <<  "Done receiving data. Got " << position << " Bytes." << endl;
				#endif

				char xml_string[position];
				memcpy(xml_string, recv_buffer, position);
				mmp->process_monkey_data(xml_string);
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

static struct option options[] = {
	{ "help",		no_argument,		NULL, 'h' },
	{ "port",		required_argument,	NULL, 'p' },
	{ "port_rc", 	required_argument,	NULL, 'y' },
	{ NULL, 0, 0, 0 }
};

//	============================================================
//						CUSTOM FUNCTIONS
//	============================================================

static int init_rc_connections(vector<Rendering_Client> *rendering_client, vector<char*> *host_ips, int host_port)
{
	int rc = 0;

	if (SDLNet_Init() < 0)
	{
		cerr << "SDLNet_Init: " << SDLNet_GetError() << endl;
		rc = EXIT_FAILURE;
	}

	for (unsigned int i = 0; i < host_ips->size(); ++i)
	{
		IPaddress ip;
		int connection_tries = 0;
		rc = SDLNet_ResolveHost(&ip, host_ips->at(i), host_port);
		if (rc != 0)
		{
			cerr << "SDLNet_ResolveHost: " << SDLNet_GetError() << endl;
			rc = EXIT_FAILURE;
		}
		else if (rc == 0)
		{
			TCPsocket socket;
			do
			{
				SDL_Delay(1000);
				connection_tries++;
				
				#if DEBUG
					cout << "Connection Try: " << connection_tries << endl;
				#endif

				socket = SDLNet_TCP_Open(&ip);
				if (!socket)
				{
					cerr << "SDLNet_TCP_Open: " << SDLNet_GetError() << endl;
					rc = EXIT_FAILURE;
				}
			} while (socket == NULL && connection_tries < MAX_CONNECTIONS_TRIES) ;
			if (socket != NULL && connection_tries < MAX_CONNECTIONS_TRIES)
			{
				Rendering_Client ren_cli = Rendering_Client(i, socket);
				rendering_client->push_back(ren_cli);
			}
		}
	}

	return rc;
}

#endif