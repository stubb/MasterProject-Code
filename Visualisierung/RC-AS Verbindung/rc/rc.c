#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <string.h>

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
	UDPsocket sd;
	UDPpacket *p;
	int i;
	int rc = 0;

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	
	if (SDLNet_Init() < 0)
	{
		fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

	if (!(sd = SDLNet_UDP_Open(2000)))
	{
		fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

	if (!(p = SDLNet_AllocPacket(9999999)))
	{
		fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	} 

	// Preventing the minimizing of our fullscreen windows when the focus is lost.
	SDL_SetHint("SDL_VIDEO_MINIMIZE_ON_FOCUS_LOSS", "0");

	ds.num_displays = 1;//SDL_min(10, SDL_GetNumVideoDisplays());

	ds.display = SDL_malloc(sizeof(struct DisplayInfo) * ds.num_displays);
	if (ds.display == NULL)
		return EXIT_FAILURE;

	for (i = 0; i < ds.num_displays; ++i)
	{
		// Too lazy to write some proper title string handling here.
		char title[] = "Window 0";
		title[sizeof(title) - 2] += (char) i;

		ds.display[i].window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED_DISPLAY(i),
			SDL_WINDOWPOS_CENTERED_DISPLAY(i), 640, 480, 
0);//SDL_WINDOW_FULLSCREEN_DESKTOP);
		ds.display[i].renderer = SDL_CreateRenderer(ds.display[i].window, -1, 0);
		SDL_Delay(1000);

		if (ds.display[i].window == NULL)
			return EXIT_FAILURE;
	}

	// get meta_data
	int meta_data[2] = {0};
	while (rc != 1) {
		rc = SDLNet_UDP_Recv(sd, p);
		if (rc == -1)
		{
			printf("Receive of Metadata failed! Error %i\n", rc);
			return 1;
		}
	}
	meta_data[0] = p->data[3]<<24 | p->data[2]<<16 | p->data[1]<<8 | 
p->data[0];
	meta_data[1] = p->data[7]<<24 | p->data[6]<<16 | p->data[5]<<8 | 
p->data[4];
	printf("Received Meta Data. Width: %i, Height: %i\n", meta_data[0], 
meta_data[1]);
	for (int i = 0; i < ds.num_displays; ++i)
	{
		ds.display[i].texture = SDL_CreateTexture(ds.display[i].renderer,SDL_PIXELFORMAT_BGR24, SDL_TEXTUREACCESS_STREAMING, meta_data[0], meta_data[1]);
	}

	while (!PollEvents())
	{
		rc = SDLNet_UDP_Recv(sd, p);
		if (rc == -1)
		{
			printf("\nrecv failed with code %i %i...exit\n", rc, errno);
			return 1;
		}
		if (rc == 0)
		{
			//nothing
		}
		if (rc == 1)
		{
			void* image_pointer = (void *) p->data;
			
				printf("\tData: %d\n", (int) p->data[0]);
				printf("\tData: %d\n", (int) p->data[1]);
				printf("\tData: %d\n", (int) p->data[2]);
				printf("\tData: %d\n", (int) p->data[3]);
				printf("\tData: %d\n", (int) p->data[4]);
				printf("\tData: %d\n", (int) p->data[5]);
				printf("\tData: %d\n", (int) p->data[6]);
				printf("\tData: %d\n", (int) p->data[7]);
				printf("\tData: %d\n", (int) p->data[8]);
				printf("\tData: %d\n", (int) p->data[9]);
				printf("\tData: %d\n", (int) p->data[10]);
				printf("\tData: %d\n", (int) p->data[11]);
				printf("\tData: %d\n", (int) p->data[12]);
				printf("\tData: %d\n", (int) p->data[13]);
				printf("\tData: %d\n", (int) p->data[14]);
				printf("\tData: %d\n", (int) p->data[15]);
				printf("\tData: %d\n", (int) p->data[16]);
				printf("\tData: %d\n", (int) p->data[17]);
				printf("\tData: %d\n", (int) p->data[18]);
				printf("\tData: %d\n", (int) p->data[19]);
				printf("\tData: %d\n", (int) p->data[20]);
				printf("\tData: %d\n", (int) p->data[21]);
				printf("\tData: %d\n", (int) p->data[22]);
				printf("\tData: %d\n", (int) p->data[23]);
				printf("\tData: %d\n", (int) p->data[24]);
				printf("\tData: %d\n", (int) p->data[25]);
				printf("\tData: %d\n", (int) p->data[26]);
				printf("\tData: %d\n", (int) p->data[27]);
				printf("\tData: %d\n", (int) p->data[28]);
				printf("\tData: %d\n", (int) p->data[29]);
				printf("\tData: %d\n", (int) p->data[30]);
				printf("\tData: %d\n", (int) p->data[31]);
				printf("\tData: %d\n", (int) p->data[32]);
				printf("\tData: %d\n", (int) p->data[33]);
				printf("\tData: %d\n", (int) p->data[34]);
				printf("\tData: %d\n", (int) p->data[35]);
				printf("\tData: %d\n", (int) p->data[36]);
				printf("\tData: %d\n", (int) p->data[37]);
				printf("\tData: %d\n", (int) p->data[38]);
				printf("\tData: %d\n", (int) p->data[39]);
				printf("\tData: %d\n", (int) p->data[40]); 
			
			for (int i = 0; i < ds.num_displays; ++i)
			{
				SDL_Rect slice = {i * meta_data[0] / ds.num_displays, 0, meta_data[0] / ds.num_displays, meta_data[0]};
				if (SDL_UpdateTexture(ds.display[i].texture, NULL, image_pointer, meta_data[0] * NUMBER_OF_COLOR_CHANNELS) < 0)
				{
					fprintf(stderr, "SDL_UpdateTexture: %s\n", SDL_GetError());
					exit(EXIT_FAILURE);
				}
				SDL_RenderClear(ds.display[i].renderer);
				SDL_RenderCopy(ds.display[i].renderer, ds.display[i].texture, &slice, NULL);			
				SDL_RenderPresent(ds.display[i].renderer);
				}
			}
			SDL_Delay(3000);
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
