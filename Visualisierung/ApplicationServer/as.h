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

#include "Rendering_Client.h"
#include "MonkeyMediaProcessor.h"

/*	Maximum Connection tries for Connection to RC.	*/
#define MAX_CONNECTIONS_TRIES 2

using namespace cv;
using namespace std;

//	============================================================
//						GLOBAL VARIABLES
//	============================================================
extern MonkeyMediaProcessor *mmp;

static struct option options[] = {
	{ "help",		no_argument,		NULL, 'h' },
	{ "port",		required_argument,	NULL, 'p' },
	{ "port_rc", 	required_argument,	NULL, 'y' },
	{ NULL, 0, 0, 0 }
};

//	============================================================
//						CUSTOM FUNCTIONS
//	============================================================

static int init_rc_connections(vector<Rendering_Client*> *rendering_client, vector<char*> *host_ips, int host_port)
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
				Rendering_Client *ren_cli = new Rendering_Client(i, socket);
				rendering_client->push_back(ren_cli);
			}
		}
	}

	return rc;
}

#endif