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
	XMLDocument document;
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
		
		Uint8 pixel_data[image_dimensions[0] * image_dimensions[1] * NUMBER_OF_COLOR_CHANNELS];
		char current_pixel_value_buffer[4];
		current_pixel_value_buffer[4] = '\0';
		
		for (int i = 0; i < image_dimensions[0] * image_dimensions[1] * NUMBER_OF_COLOR_CHANNELS; ++i)
		{
			std::strncpy(current_pixel_value_buffer, data->GetText()+i*3, 3);
			char * pEnd;
			pixel_data[i] = (Uint8) strtol(current_pixel_value_buffer, &pEnd, 10);
		}
		
#if DEBUG
		for (int i = 0; i < image_dimensions[0] * image_dimensions[1] * NUMBER_OF_COLOR_CHANNELS; ++i)
		{
			printf("%d\n", pixel_data[i]);
		}
#endif
			
			
		for (int i = 0; i < NUMBER_OF_RENDERING_CLIENTS; ++i)
		{
			printf("hiho\n");
			rc = SDLNet_TCP_Send (tcpsock, pixel_data, (image_dimensions[0] * image_dimensions[1] * NUMBER_OF_COLOR_CHANNELS) + 1);
			if(rc < image_dimensions[0] * image_dimensions[1] * NUMBER_OF_COLOR_CHANNELS) 
			{
				printf("Sending data failed with code %i.\n", rc);
			}
		}
	}
}

int main(int argc, char** argv)
{
	if (SDLNet_Init() < 0)
	{
		fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}
	
	if(SDLNet_ResolveHost(&ip, "localhost", 2000)==-1) {
		printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
		exit(1);
	}

	tcpsock=SDLNet_TCP_Open(&ip);
	if(!tcpsock) {
		printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
		exit(2);
	}

	receive_server.set_message_handler(&on_message);
	receive_server.init_asio();
	receive_server.listen(9002);
	receive_server.start_accept();
	receive_server.run();
}
