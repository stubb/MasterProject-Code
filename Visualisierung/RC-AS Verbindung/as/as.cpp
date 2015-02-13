#include <iostream>
#include <unistd.h>
#include <string>
#include <inttypes.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include "tinyxml2.h"

#define DEBUG 1
#define NUMBER_OF_COLOR_CHANNELS 3
#define NUMBER_OF_RENDERING_CLIENTS 1

using namespace cv;
using namespace std;
using namespace tinyxml2;

typedef websocketpp::server<websocketpp::config::asio> server;
server receive_server;

IPaddress ip;
TCPsocket tcpsock;

int rc = 0;

void on_message(websocketpp::connection_hdl hdl, server::message_ptr msg) {
	tinyxml2::XMLDocument document;
	document.Parse(msg->get_payload().c_str());
	
	//TODO alles :D
	//get meta data
	int image_dimensions[2] = {0};
	document.FirstChildElement( "package" )->FirstChildElement( "width" )->QueryIntText( &image_dimensions[0] );
	document.FirstChildElement( "package" )->FirstChildElement( "height" )->QueryIntText( &image_dimensions[1] );
	std::cout << image_dimensions[0] << " " << image_dimensions[1] << std::endl;
	
	// send meta data
	rc = SDLNet_TCP_Send (tcpsock, image_dimensions, sizeof(int) * 2);
	if(rc < (signed) sizeof(int) * 2) 
	{
		printf("Sending metadata failed with code %i.\n", rc);
	}
	else
	{
		XMLElement* data = document.FirstChildElement( "package" )->FirstChildElement( "data" );
		
		char pixel_data[image_dimensions[0] * image_dimensions[1] * NUMBER_OF_COLOR_CHANNELS];
		char * pEnd = (char *) data->GetText();

		for (int i = 0; i < image_dimensions[0] * image_dimensions[1] * NUMBER_OF_COLOR_CHANNELS; ++i)
		{		
			pixel_data[i] = strtol(pEnd, &pEnd, 10);
		}
		
#if DEBUG
		for (int i = 0; i < 25; ++i)
		{
			std::cout << pixel_data[i] << std::endl;
		}
#endif
			
			
		for (int i = 0; i < NUMBER_OF_RENDERING_CLIENTS; ++i)
		{
			rc = SDLNet_TCP_Send (tcpsock, pixel_data, (image_dimensions[0] * image_dimensions[1] * NUMBER_OF_COLOR_CHANNELS) + 1);
			if(rc < image_dimensions[0] * image_dimensions[1] * NUMBER_OF_COLOR_CHANNELS) 
			{
				printf("Sending data failed with code %i.\n", rc);
			}
		}
	}
}

static int PollEvents()
{
	SDL_Event event;
	
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			return 1;	
		}
		else if (event.type == SDL_KEYDOWN)
		{
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				return 1;
			}
		}
		else if (event.type == SDL_WINDOWEVENT)
		{
			//printf("WinEvent!\n");
		}
	}
	return 0;
}

int main(int argc, char** argv)
{
	char* host_ip = "localhost";
	if(argc > 1) {
		host_ip = argv[1];
	}
	
	if (SDLNet_Init() < 0)
	{
		fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}
	
	if(SDLNet_ResolveHost(&ip, host_ip, 2000)==-1) {
		printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
		exit(1);
	}

	tcpsock=SDLNet_TCP_Open(&ip);
	if(!tcpsock) {
		printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
		exit(2);
	}
	websocketpp::lib::error_code error;
	receive_server.set_message_handler(&on_message);
	receive_server.init_asio(error);
	std::cout << error << std::endl;
	receive_server.listen(9002, error);
	std::cout << error << std::endl;
	receive_server.start_accept();
	receive_server.run();
	//receive_server.stop_listening();
}
