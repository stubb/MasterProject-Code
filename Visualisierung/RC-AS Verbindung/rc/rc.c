#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <SDL2/SDL.h>
#include <string.h>

#define DEBUG 0

#if defined _WIN32
	#include "dirent.h"
	#include <WinSock2.h>
	#pragma comment(lib, "libws2_32.a")
#else
	#include <dirent.h>
	#include <sys/socket.h>
	#include <sys/types.h>
#endif

#if defined _WIN32
	#include <SDL2/SDL_image.h>
#elif defined __linux__
	#include <SDL2/SDL_image.h>
#else
	#include <SDL2_image/SDL_image.h>
#endif


struct DisplayInfo {
	SDL_Window *window;
	SDL_Rect bounds;
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

/* Draws the display positions for each display. */
static void DrawSurface(int windowID, SDL_Surface* image)
{
	SDL_Window *window;
	SDL_Surface *surface;

	window = SDL_GetWindowFromID(windowID);
	if (window == NULL)
	{
		printf("Creating window failed!");
		return;
	}

	surface = SDL_GetWindowSurface(window);
	if (surface == NULL)
	{
		printf("Creating surface failed!");
		return;
	}

	int sectionWidth = image->w / ds.num_displays;
	//printf("Section Width: %i\n", sectionWidth);

	for (int i = 0; i < ds.num_displays; i++)
	{
		if (SDL_GetWindowID(ds.display[i].window) == windowID)
		{
			if (i == 0)
				windowIdNormalizer = windowID;

			SDL_Rect scale = { .x = (windowID - windowIdNormalizer) * sectionWidth, .y = 0, .w = sectionWidth, .h = image->h };
			//printf("normalizer: %i\n", windowIdNormalizer);
			//printf("%i, %i, %i, %i\n\n", scale.x, scale.y, scale.w, scale.h);
			if (SDL_BlitScaled(image, &scale, surface, NULL) != 0)
			{
				printf("ScaleFail");
				return;
			}
			SDL_FreeSurface(surface);
		}
	}
	if (SDL_UpdateWindowSurface(window) != 0)
	{
		printf("Update Surface Failed!");
		return;
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

int main(int argc, char *argv[])
{
	//return;
	int i;

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

	// Preventing the minimizing of our fullscreen windows when the focus is lost.
	SDL_SetHint("SDL_VIDEO_MINIMIZE_ON_FOCUS_LOSS", "0");

	ds.num_displays = SDL_min(10, SDL_GetNumVideoDisplays());

	ds.display = SDL_malloc(sizeof(struct DisplayInfo) * ds.num_displays);
	if (ds.display == NULL)
		return EXIT_FAILURE;

	for (i = 0; i < ds.num_displays; i++)
	{
		// Too lazy to write some proper title string handling here.
		char title[] = "Window 0";
		title[sizeof(title) - 2] += (char) i;

		ds.display[i].window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED_DISPLAY(i),
			SDL_WINDOWPOS_CENTERED_DISPLAY(i), 320, 240, 0/*SDL_WINDOW_FULLSCREEN_DESKTOP*/);
		
		SDL_Delay(1000);

		if (ds.display[i].window == NULL)
			return EXIT_FAILURE;

		// This sets up some numbers for the drawing operation in DrawSurface.
		if (SDL_GetDisplayBounds(i, &ds.display[i].bounds) < 0)
			return EXIT_FAILURE;

		printf("flags %d\n\n",SDL_GetWindowFlags( ds.display[i].window ));
		// if (SDL_SetWindowFullscreen(ds.display[i].window,  SDL_WINDOW_FULLSCREEN_DESKTOP) != 0)
		// {
		// 	printf("shita\n");
		// }
	}

	SOCKET s, s1;
	int rc;	//Rueckgabewert
	struct sockaddr_in addr;	//IPv4 Adresse
	{
		WSADATA wsaData;
		rc = WSAStartup(MAKEWORD(2,2), &wsaData);
	}

	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = ADDR_ANY;	//Akzeptiere jede Adresse
	addr.sin_port = htons(11111);


	rc = bind(s, (struct sockaddr*) &addr, sizeof(addr));	//muesste noch ausgewertet werden

	printf("Server Socket is listening...\n");
	rc = listen(s, 10);

	printf("accepting connection...\n");
	s1 = accept(s, NULL, NULL);

	int meta_data[3] = {0};
	rc = recv(s1, meta_data, 3 * sizeof(int), 0);

	if (rc < 0)
	{
		printf("Receive of Metadata failed! Error %i\n", rc);
		return 1;
	}
	else
		printf("Received Meta Data.\n");

	while (!PollEvents())
	{
		unsigned char image[3 * meta_data[1] * meta_data[2]];

		#if DEBUG
			printf("Sizeof(image) = %i\n", sizeof(image));
		#endif

		for (int i = 0; i < meta_data[0]; i++)
		{
			#if DEBUG
				printf("Position %i with Length %i\n", meta_data[1] * meta_data[2] / meta_data[0] * i, 3 * meta_data[1] * meta_data[2] / meta_data[0]);
			#endif

			rc = recv(s1, image + 3 * meta_data[1] * meta_data[2] / meta_data[0] * i, 3 * meta_data[1] * meta_data[2] / meta_data[0], 0);
			if (rc < 0)
			{
				printf("recv failed with code %i %i...exit\n", rc, errno);
				return 1;
			}
		}
		for (int i = 0; i < ds.num_displays; i++)
		{

			SDL_Surface *img = SDL_CreateRGBSurfaceFrom((void *)image, meta_data[1], meta_data[2], 24, 1280 * 3, 0xFF0000, 0x00FF00, 0x0000FF, 0xFFFFFF);
			if (img == NULL)
			{
				printf("Convert of Image failed!\n");
				return 1;
			}
			
			DrawSurface(SDL_GetWindowID(ds.display[i].window), img);
		}
	}

	SDL_Quit();
	SDL_free(ds.display);
	return EXIT_SUCCESS;
}