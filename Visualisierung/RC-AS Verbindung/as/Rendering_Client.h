#ifndef __RENDERING_CLIENT_H__
#define __RENDERING_CLIENT_H__

/*	SDL2 Includes.	*/
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

class Rendering_Client
{
	private:
		/*	Consecutive number giving the Order of the Rendering Clients.	*/
		int identifier;
		/*	Socket to the Rendering Client.	*/
		TCPsocket rc_tcpsock;

	public:
		Rendering_Client(int id, TCPsocket s)
		{
			identifier = id;
			rc_tcpsock = s;
		}
};

#endif