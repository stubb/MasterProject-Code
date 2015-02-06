#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <SDL2/SDL.h>
#include <string.h>

#if defined _WIN32
	#include "dirent.h"
	#include <WinSock2.h>
	#pragma comment(lib, "libws2_32.a")
#else
	#include <dirent.h>
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <netinet/in.h>
	#include <unistd.h> /* for close() for socket */ 
	#include <stdlib.h>
#endif

#define DEBUG 1
#define NUMBER_OF_COLOR_CHANNELS 3

struct DisplayInfo {
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
};

struct DisplaySetup {
	int num_displays;
	int xmin, xmax;
	int ymin, ymax;
	int width, height;
	struct DisplayInfo *display;
};

static struct DisplaySetup ds;

int windowIdNormalizer = 1;

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
	int i;

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

	// Preventing the minimizing of our fullscreen windows when the focus is lost.
	SDL_SetHint("SDL_VIDEO_MINIMIZE_ON_FOCUS_LOSS", "0");

	ds.num_displays = SDL_min(10, SDL_GetNumVideoDisplays());

	ds.display = SDL_malloc(sizeof(struct DisplayInfo) * ds.num_displays);
	if (ds.display == NULL)
		return EXIT_FAILURE;

	for (i = 0; i < ds.num_displays; ++i)
	{
		// Too lazy to write some proper title string handling here.
		char title[] = "Window 0";
		title[sizeof(title) - 2] += (char) i;

		ds.display[i].window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED_DISPLAY(i),
			SDL_WINDOWPOS_CENTERED_DISPLAY(i), 640, 480, /*0*/ SDL_WINDOW_FULLSCREEN_DESKTOP);
		ds.display[i].renderer = SDL_CreateRenderer(ds.display[i].window, -1, 0);
		SDL_Delay(1000);

		if (ds.display[i].window == NULL)
			return EXIT_FAILURE;
	}

	int rc;	//Rueckgabewert

#ifdef __WIN32__
	SOCKET s, s1;
	struct sockaddr_in addr;	//IPv4 Adresse
	{
		WSADATA wsaData;
		rc = WSAStartup(MAKEWORD(2,2), &wsaData);
	}
#else
	int s;
	int s1;
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof addr);
#endif

	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	addr.sin_family = AF_INET;

#ifdef __WIN32__
	addr.sin_addr.S_un.S_addr = ADDR_ANY;	//Akzeptiere jede Adresse
#else
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
#endif

	addr.sin_port = htons(11111);


	rc = bind(s, (struct sockaddr*) &addr, sizeof(addr));	//muesste noch ausgewertet werden

	printf("Server Socket is listening...\n");
	rc = listen(s, 10);

	printf("accepting connection...\n");
	s1 = accept(s, NULL, NULL);

	int meta_data[2] = {0};
	rc = recv(s1, meta_data, 2 * sizeof(int), 0);

	if (rc < 0)
	{
		printf("Receive of Metadata failed! Error %i\n", rc);
		return 1;
	}
	else
	{
		printf("Received Meta Data. Width: %i, Height: %i\n", meta_data[0], meta_data[1]);
		for (int i = 0; i < ds.num_displays; ++i)
		{
			ds.display[i].texture = SDL_CreateTexture(ds.display[i].renderer, SDL_PIXELFORMAT_BGR24, SDL_TEXTUREACCESS_STREAMING, meta_data[0], meta_data[1]);
		}
	}

	while (!PollEvents())
	{
		// RGB * width * height
		int numberOfPixelValues = NUMBER_OF_COLOR_CHANNELS * meta_data[0] * meta_data[1];
		unsigned char image[numberOfPixelValues];

		int position = 0;
		do
		{
			rc = recv(s1, image + position, numberOfPixelValues - position, 0);
			if (rc < 0)
			{
				printf("recv failed with code %i %i...exit\n", rc, errno);
				return 1;
			}

			position += rc;

		} while (rc != 0) ;
		for (int i = 0; i < ds.num_displays; ++i)
		{
			SDL_Rect slice = {i * meta_data[0] / ds.num_displays, 0, meta_data[0] / ds.num_displays, meta_data[0]};
			SDL_UpdateTexture(ds.display[i].texture, NULL, (void *)image, meta_data[0] * NUMBER_OF_COLOR_CHANNELS);
			SDL_RenderClear(ds.display[i].renderer);
			SDL_RenderCopy(ds.display[i].renderer, ds.display[i].texture, &slice, NULL);
			SDL_RenderPresent(ds.display[i].renderer);
		}
	}

	for (int i = 0; i < ds.num_displays; ++i)
	{
		SDL_DestroyTexture(ds.display[i].texture);
		SDL_DestroyRenderer(ds.display[i].renderer);
		SDL_DestroyWindow(ds.display[i].window);
		ds.display[i].texture = NULL;
		ds.display[i].renderer = NULL;
		ds.display[i].window = NULL;
	}
	SDL_free(ds.display);
	SDL_Quit();
	return EXIT_SUCCESS;
}
