#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <string.h>
#include <inttypes.h>

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
	IPaddress ip;
	TCPsocket tcpsock;
	TCPsocket client = NULL;
	SDLNet_SocketSet set;
	int num_used_sockets = 0;
	int i;
	int rc = 0;
	int listening_port = 2000;

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	
	if (SDLNet_Init() < 0)
	{
		fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

	// one client
	set = SDLNet_AllocSocketSet(1);
	if(!set) {
		printf("SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
		exit(1);
	}

	if(SDLNet_ResolveHost(&ip, NULL, listening_port)==-1) {
		printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
		exit(1);
	}
	printf("Listening on port %d\n", listening_port);

	tcpsock = SDLNet_TCP_Open(&ip);
	if(!tcpsock) {
		printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
		exit(2);
	}

	// wait for client
	while (client == NULL) {
		client = SDLNet_TCP_Accept(tcpsock);
		if(!client) {
			printf("SDLNet_TCP_Accept: %s\n", SDLNet_GetError());
			SDL_Delay (1000);
		}
	}

	num_used_sockets = SDLNet_TCP_AddSocket(set, client);
	if(num_used_sockets == -1) {
		printf("SDLNet_AddSocket: %s\n", SDLNet_GetError());
	}
	
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
			SDL_WINDOWPOS_CENTERED_DISPLAY(i), 640, 480, 0);//SDL_WINDOW_FULLSCREEN_DESKTOP);
		ds.display[i].renderer = SDL_CreateRenderer(ds.display[i].window, -1, 0);
		SDL_Delay(1000);

		if (ds.display[i].window == NULL)
			return EXIT_FAILURE;
	}
	
	// get meta_data
	int meta_data[2] = {0};
	while (rc <= 0) {
		rc = SDLNet_TCP_Recv(client, meta_data, sizeof(int) * 2);
		if (rc <= 0)
		{
			printf("Receive of Metadata failed! Error %i\n", rc);
			SDL_Delay(1000);
		}
	}
	printf("%i %i\n", meta_data[0], meta_data[1]);

	for (int i = 0; i < ds.num_displays; ++i)
	{
		ds.display[i].texture = SDL_CreateTexture(ds.display[i].renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, meta_data[0], meta_data[1]);
	}

	char img_data[(meta_data[0] * meta_data[1] * NUMBER_OF_COLOR_CHANNELS) + 1];
	
	while (!PollEvents())
	{
		int numready = SDLNet_CheckSockets(set, 1000);
		if(numready == -1) {
			printf("SDLNet_CheckSockets: %s\n", SDLNet_GetError());
			//most of the time this is a system error, where perror might help you.
			perror("SDLNet_CheckSockets");
		}
		else if(numready) {
			printf("There are %d sockets with activity!\n",numready);
			// check all sockets with SDLNet_SocketReady and handle the active ones.
			if(SDLNet_SocketReady(client)) {

				int numberOfPixelValues = NUMBER_OF_COLOR_CHANNELS * meta_data[0] * meta_data[1];
				int position = 0;
				do
				{
					rc = SDLNet_TCP_Recv(client, img_data + position, numberOfPixelValues - position);
					if (rc < 0)
					{
						printf("SDLNet_TCP_Recv failed with Code %i %i ...exit\n", rc, errno);
						return 1;
					}
					position += rc;

				} while (rc != 0) ;

				for (int i = 0; i < ds.num_displays; ++i)
				{
					SDL_Rect slice = {i * meta_data[0] / ds.num_displays, 0, meta_data[0] / ds.num_displays, meta_data[0]};
					if (SDL_UpdateTexture(ds.display[i].texture, NULL, (void *) img_data, meta_data[0] * NUMBER_OF_COLOR_CHANNELS) < 0)
					{
						fprintf(stderr, "SDL_UpdateTexture: %s\n", SDL_GetError());
						exit(EXIT_FAILURE);
					}
					SDL_RenderClear(ds.display[i].renderer);
					SDL_RenderCopy(ds.display[i].renderer, ds.display[i].texture, &slice, NULL);			
					SDL_RenderPresent(ds.display[i].renderer);
				}
			}
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
	SDLNet_TCP_Close(client);
	SDLNet_TCP_Close(tcpsock);
	SDLNet_FreeSocketSet(set);
	set=NULL;
	SDLNet_Quit();
	SDL_Quit();
	return EXIT_SUCCESS;
}
