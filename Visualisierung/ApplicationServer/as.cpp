/*	Debug Mode.	*/
#define DEBUG 0

/*	Picture Settings.	*/
#define NUMBER_OF_COLOR_CHANNELS 3

/*	Standard Includes.	*/
#include <iostream>
#include <string>

/*	OpenCV Includes.	*/
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

/*	SDL2 Includes.	*/
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

/*	User Functions.	*/
#include "NetworkHandler.h"
#include "Rendering_Client.h"
#include "MonkeyMediaProcessor.h"

// global var definitions
MonkeyMediaProcessor *mmp = NULL;

int main(int argc, char** argv)
{
	vector<Rendering_Client*> *rendering_clients = new vector<Rendering_Client*>();
	int port = 9000;

	if (SDLNet_Init() < 0)
	{
		cerr << "SDLNet_Init: " << SDLNet_GetError() << endl;
		exit(1);
	}

	if (argc < 4 || (argc % 2 != 0))
	{
		cerr << "Usage: Applicationserver port renderingclient_ip1 rendering_client_port1 ...\n" << endl;
		exit(1);
	}

	port = atoi(argv[2]);

	int name = 0;
	// extract all rc infos
	for (int i = 2; i < argc; i += 2)
	{
		#if DEBUG
			cout << "Init Rendering_Client IP: " << argv[i] << " Port: " << argv[i + 1] << endl;
		#endif
		Rendering_Client *rc = new Rendering_Client(name);
		if(rc->init(argv[i], atoi(argv[i + 1])))
			rendering_clients->push_back(rc);
		name++;
	}

	if (rendering_clients->size() > 0)
	{
		mmp = new MonkeyMediaProcessor(rendering_clients);
		NetworkHandler* nwh = new NetworkHandler(port);
		nwh->run();
		nwh->destroy();
	}
	else
	{
		cerr << "Couldn't establish Connections. Shutting down " << argv[0] << "." << endl;
		exit(1);
	}
	return(EXIT_SUCCESS);
}