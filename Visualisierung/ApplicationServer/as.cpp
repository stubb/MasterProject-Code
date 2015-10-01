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

/*	SDL Includes.	*/
#if defined _WIN32
	#include <SDL2/SDL.h>
	#include <SDL2/SDL_net.h>
#elif defined __linux__
	#include <SDL2/SDL.h>
	#include <SDL2/SDL_net.h>
#else
	#include <SDL2/SDL.h>
	#include <SDL2_net/SDL_net.h>
#endif

/*	XML Includes.	*/
#include "tinyxml2.h"

/*	User Functions.	*/
#include "NetworkHandler.h"
#include "Rendering_Client.h"
#include "MonkeyMediaProcessor.h"

/*	Hilfsmethoden.	*/
#include "Utility.h"

// global var definitions
MonkeyMediaProcessor *mmp = NULL;

using namespace std;
using namespace tinyxml2;

int main(int argc, char** argv)
{
	vector<Rendering_Client*> *rendering_clients = new vector<Rendering_Client*>();
	int port = 9000;

	if (SDLNet_Init() < 0)
	{
		cerr << "SDLNet_Init: " << SDLNet_GetError() << endl;
		exit(1);
	}

	if (argc != 2)
	{
		cerr << "Usage: Applicationserver [PATH TO CONFIG]\n" << endl;
		exit(1);
	}	
	
	string config_string;
	try
	{
		config_string = get_file_as_string(argv[1]);
		XMLDocument config_xml;
		config_xml.Parse(config_string.c_str(), (unsigned int) config_string.size());
		if (config_xml.Error()) {
			cout << "XML Parse() ErrorID: " << config_xml.ErrorID() << endl;
			exit(1);
		}

		config_xml.FirstChildElement("package")->FirstChildElement("ApplicationServer")->QueryIntAttribute("port", &port);
		#if DEBUG
			cout << "Application Server Port: " << port << endl;
		#endif
		if (port == 0)
		{
			cerr << "Couldn't read AS-Port from config File.\n" << endl;
			exit(1);
		}
		// extract all rc infos
		int name = 0;
		XMLElement *rendering_client_list = config_xml.FirstChildElement("package")->FirstChildElement("RenderingClients");
		for (XMLElement *child = rendering_client_list->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
		{
			Rendering_Client *rc = new Rendering_Client(name);
			int rc_port;
			int rc_xpos;
			int rc_ypos;
			child->FirstChildElement("port")->QueryIntText(&rc_port);
			child->FirstChildElement("xpos")->QueryIntText(&rc_xpos);
			child->FirstChildElement("ypos")->QueryIntText(&rc_ypos);
			#if DEBUG
				cout << "Init Rendering_Client IP: " << child->FirstChildElement("ip")->GetText() <<
					" Port: " << rc_port << 
					" XPos: " << rc_xpos <<
					" Ypos: " << rc_ypos << endl;
			#endif
			if(rc->init(child->FirstChildElement("ip")->GetText(), rc_port, rc_xpos, rc_ypos))
				rendering_clients->push_back(rc);
			name++;
		}
	
	}
	catch(...)
	{
		cerr << "Error loading config File"  << endl;
	}

	if (rendering_clients->size() > 0)
	{
		mmp = new MonkeyMediaProcessor(rendering_clients);
		mmp->init_clientship();
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