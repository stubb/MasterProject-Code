#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <SDL2/SDL.h>
#include <string.h>
#include <time.h>

#define DEBUG 1

#if defined _WIN32
	#include "dirent.h"
	#include <WinSock2.h>
	#pragma comment(lib, "libws2_32.a")
#else
	#include <dirent.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <unistd.h>
	#include <arpa/inet.h>
#endif

#if defined _WIN32
	#include <SDL2/SDL_image.h>
#elif defined __linux__
	#include <SDL2/SDL_image.h>
#else
	#include <SDL2_image/SDL_image.h>
#endif

int random(int min, int max) {
  return rand() % (max - min + 1) + min;
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

int main(int argc, char *argv[])
{
	int MAX_CONNECTION_TRIES = 10;
	
	const int SCREEN_FPS = 24;
	const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;
	
	SDL_Window *sdlWindow;
	SDL_Renderer *sdlRenderer;
	    sdlWindow = SDL_CreateWindow(
        "test window",                  // window title
        SDL_WINDOWPOS_UNDEFINED,           // initial x position
        SDL_WINDOWPOS_UNDEFINED,           // initial y position
        640,                               // width, in pixels
        480,                               // height, in pixels
        SDL_WINDOW_OPENGL                  // flags - see below
    );

    // Check that the window was successfully made
    if (sdlWindow == NULL) {
        // In the event that the window could not be made...
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED);

	int rc;	//Rueckgabewert
	struct sockaddr_in addr; //IPv4 Adresse
#ifdef __WIN32__
	SOCKET s;
	{
		WSADATA wsaData;
		rc = WSAStartup(MAKEWORD(2,2), &wsaData);
	}
#else
	int s;
	memset(&addr, 0, sizeof addr);
#endif

	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	addr.sin_family = AF_INET;
#ifdef __WIN32__
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
#else
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
#endif
	addr.sin_port = htons(11111);

	int connection_tries = 0;
	do
	{
		connection_tries++;
		rc = connect(s, (struct sockaddr*)&addr, sizeof(addr));
		SDL_Delay(100);
	} while (rc < 0 && connection_tries <= MAX_CONNECTION_TRIES) ;

	if (rc < 0)
	{
		fprintf(stderr, "Couldn't connect to Rendering Client. Max tries exceeded.\n");
		return 1;
	}
	else {
		srand(time(0));
		// frame limiter stuff
		Uint32 mStartTicksFPS;
		Uint32 mStartTicksCAP;
		int countedFrames = 0;
		mStartTicksFPS = SDL_GetTicks();
		while (!PollEvents())
		{
			mStartTicksCAP = SDL_GetTicks();
			
			// generate something
			SDL_SetRenderDrawColor(sdlRenderer, random(0, 255), random(0, 255), random(0, 255), 255);
			
			 //Calculate and correct fps
			float avgFPS = countedFrames / ( (SDL_GetTicks() - mStartTicksFPS) / 1000.f );
			if( avgFPS > 2000000 ) {
				avgFPS = 0;
			}
			
			printf("Avg frames with cap %.0f\n", avgFPS);
			
			// get Metadata & send
			
			
			// display it
			SDL_RenderClear(sdlRenderer);
			SDL_RenderPresent(sdlRenderer);
			++countedFrames;
			//If frame finished early
			int frameTicks = (SDL_GetTicks() - mStartTicksCAP);
			if( frameTicks < SCREEN_TICKS_PER_FRAME ) {
				//Wait remaining time
				SDL_Delay( SCREEN_TICKS_PER_FRAME - frameTicks );
			}
		}
	}


#ifdef __WIN32__
	WSACleanup;
#endif
	SDL_DestroyRenderer(sdlRenderer);
	SDL_DestroyWindow(sdlWindow);
	SDL_Quit();
	return EXIT_SUCCESS;
}
