#include <iostream>
#include <unistd.h>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include "tinyxml2.h"


#define DEBUG 0

using namespace cv;
using namespace std;
using namespace tinyxml2;

typedef websocketpp::server<websocketpp::config::asio> server;
server receive_server;

IPaddress ip;
TCPsocket tcpsock;

int rc = 0;
int numberOfRenderingClients = 1;

void on_message(websocketpp::connection_hdl hdl, server::message_ptr msg) {
	XMLDocument document;
	document.Parse(msg->get_payload().c_str());
	
	//TODO alles :D
	XMLElement* width = document.FirstChildElement( "package" )->FirstChildElement( "width" );
	XMLElement* height = document.FirstChildElement( "package" )->FirstChildElement( "height" );
	std::cout << width->GetText() << " xxx " << height->GetText() << std::endl;
	
	// send meta data	
	int meta_data[2] = {0};
	sscanf(width->GetText(), "%d", &meta_data[0]);
	sscanf(height->GetText(), "%d", &meta_data[1]);
	std::cout << meta_data[0] << " " << meta_data[1] << std::endl;
	
	rc = SDLNet_TCP_Send (tcpsock, meta_data, sizeof(int) * 2);
	if(rc < sizeof(int) * 2) 
	{
		printf("Sending metadata failed with code %i.\n", rc);
	}
	else
	{
		XMLElement* data = document.FirstChildElement( "package" )->FirstChildElement( "data" );
		std::cout << data->GetText() << std::endl;
				
		for (int i = 0; i < numberOfRenderingClients; ++i)
		{
			rc = SDLNet_TCP_Send (tcpsock, data->GetText(), meta_data[0] * meta_data[1] * 4);
			if(rc < meta_data[0] * meta_data[1] * 4) 
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
