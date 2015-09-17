#ifndef __RENDERING_CLIENT_H__
#define __RENDERING_CLIENT_H__

/*	Maximum Connection tries for Connection to RC.	*/
#define MAX_CONNECTIONS_TRIES 2

/*	Standard Includes.	*/
#include <iostream>

/*	SDL2 Includes.	*/
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

using namespace std;

class Rendering_Client
{
	private:
		/*	Consecutive number giving the Order of the Rendering Clients.	*/
		int identifier;
		/*	Horizontal Position	*/
		int xpos;
		/*	Vertical Position	*/
		int ypos;
		/*	Socket to the Rendering Client.	*/
		TCPsocket socket = NULL;
		/* IP with port*/
		IPaddress ip;

	public:
		Rendering_Client(int id)
		{
			identifier = id;
		}

		int get_xpos() const
		{
			return xpos;
		}
		
		int get_ypos() const
		{
			return ypos;
		}
		
		TCPsocket get_socket() const
		{
			return socket;
		}

		int init(const char* rc_ip, int rc_port, int new_xpos, int new_ypos)
		{
			xpos = new_xpos;
			ypos = new_ypos;
			int rc = 0;
			rc = SDLNet_ResolveHost(&ip, rc_ip, rc_port);
			if (rc != 0)
			{
				cerr << "SDLNet_ResolveHost: " << SDLNet_GetError() << endl;
			}
			else if (rc == 0)
			{
				int connection_tries = 0;
				do
				{
					socket = SDLNet_TCP_Open(&ip);
					if (!socket)
					{
						cerr << "SDLNet_TCP_Open: " << SDLNet_GetError() << endl;
					}
					connection_tries++;
				} while (socket == NULL && connection_tries < MAX_CONNECTIONS_TRIES) ;
			}
			if (socket != NULL)
			{
				rc = 1;
			}
			else
			{
				rc = 0;
			}
			return rc;
		}
};

#endif