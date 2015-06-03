#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <string.h>
#include <inttypes.h>
#include <stdint.h>

#define DEBUG 1

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
int texture_width = 1280;
int texture_height = 720;

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
	// Addresses.
	IPaddress ip_adress;
	int listening_port = 2000;

	if (argc == 2) {
		listening_port = atoi(argv[1]);
	}

	// Sockets.
	TCPsocket server_socket = NULL;
	TCPsocket client_socket = NULL;

	// Socketset for holding all Connections.
	SDLNet_SocketSet socket_set;

	// Returncodes and helper Variables.
	int num_used_sockets = 0;
	int i;
	int rc = 0;

	// Init SDL System.
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	if (SDLNet_Init() < 0)
	{
		fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

	// One Client.
	socket_set = SDLNet_AllocSocketSet(1);
	if(!socket_set)
	{
		fprintf(stderr, "SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

	// NULL makes Host to self -> Socket becomes Server.
	if (SDLNet_ResolveHost(&ip_adress, NULL, listening_port) == -1)
	{
		fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}
	printf("Listening on Port: %d\n", listening_port);

	// Open Socket as Server.
	server_socket = SDLNet_TCP_Open(&ip_adress);
	if(!server_socket)
	{
		fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

	// Wait for Clients.
	while (client_socket == NULL)
	{
		client_socket = SDLNet_TCP_Accept(server_socket);
		if(!client_socket)
		{
			#if DEBUG
			printf("SDLNet_TCP_Accept: %s\n", SDLNet_GetError());
			#endif
			SDL_Delay (1000);
		}
	}

	num_used_sockets = SDLNet_TCP_AddSocket(socket_set, client_socket);
	if(num_used_sockets == -1)
	{
		fprintf(stderr, "SDLNet_AddSocket: %s\n", SDLNet_GetError());
	}
	
	// Preventing the minimizing of our fullscreen windows when the focus is lost.
	SDL_SetHint("SDL_VIDEO_MINIMIZE_ON_FOCUS_LOSS", "0");

	// Get the Number of connected Displays.
	ds.num_displays = SDL_min(10, SDL_GetNumVideoDisplays());

	// Allocate Memeory for our Display "Objects".
	ds.display = SDL_malloc(sizeof(struct DisplayInfo) * ds.num_displays);
	if (ds.display == NULL)
	{
		fprintf(stderr, "No Displays found!\n'");
		exit(EXIT_FAILURE);
	}

	// Initialize Windows.
	for (i = 0; i < ds.num_displays; ++i)
	{
		ds.display[i].texture = NULL;

		// Too lazy to write some proper title string handling here.
		char title[] = "Window 0";
		title[sizeof(title) - 2] += (char) i;

		ds.display[i].window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED_DISPLAY(i), SDL_WINDOWPOS_CENTERED_DISPLAY(i), 640, 480, 0);//SDL_WINDOW_FULLSCREEN_DESKTOP);
		ds.display[i].renderer = SDL_CreateRenderer(ds.display[i].window, -1, 0);
		SDL_Delay(1000);

		if (ds.display[i].window == NULL)
		{
			fprintf(stderr, "Couldn't init Window #%d (%s)", i, title);
			exit(EXIT_FAILURE);
		}
	}
	for (i = 0; i < ds.num_displays; ++i)
	{
		SDL_SetRenderDrawColor(ds.display[i].renderer, 0, 0, 0, 255 );
		SDL_RenderPresent(ds.display[i].renderer);
	}

	int number_of_meta_informations = 6;
	int meta_informations_size = number_of_meta_informations * sizeof(int);
	int number_of_color_channels = 3;
	int *recv_buffer = (int*) malloc(1280 * 720 * 3 * sizeof(int)); // Maximum allowed Size.
	char *image_data = NULL;
	int number_of_bytes = meta_informations_size;
	int position = 0;
	int width = 0;
	int height = 0;

	while (!PollEvents())
	{
		// Check Network for Activity.
		int numready = SDLNet_CheckSockets(socket_set, 1000);
		if(numready == -1)
		{
			printf("SDLNet_CheckSockets: %s\n", SDLNet_GetError());
			//most of the time this is a system error, where perror might help you.
			perror("SDLNet_CheckSockets");
		}
		else if(numready)
		{
			#if DEBUG
			printf("There are %d sockets with activity!\n", numready);
			#endif

			// check all sockets with SDLNet_SocketReady and handle the active ones.
			if(SDLNet_SocketReady(client_socket))
			{
				#if DEBUG
				printf("Starting to receive the data. Awaiting %i Bytes total.\n", number_of_bytes);
				#endif
				// Receive the Data.
				int recv_cycle = 0;
				do
				{
					++recv_cycle;
					if (number_of_bytes - position > 0)
					{
						// Receive Data. First Cycle into the Big recv_buffer, because it could be Meta Data (int).
						// Second time we know its Picture Data, so put everything into image_data.
						if (recv_cycle < 2)
							rc = SDLNet_TCP_Recv(client_socket, recv_buffer + position, number_of_bytes - position);
						else
							rc = SDLNet_TCP_Recv(client_socket, image_data + position, number_of_bytes - position);

						// Socket Connection OK?
						if (rc < 0)
						{
							printf("SDLNet_TCP_Recv failed with Code %i %i ...exit\n", rc, errno);
							return 1;
						}

						#if DEBUG
						printf("Total Received: %i Bytes, Last Received: %i Bytes\n", position, rc);
						#endif

						// Advance Position for received Bytes.
						position += rc;
					}
					else
						rc = 0;

					// We received Meta Data, set rc to 0 to exit the Loop.
					if (recv_buffer[0] == 1337 && recv_buffer[1] == 1337 && recv_buffer[2] == 1337 && recv_buffer[3] == 1337)
					{
						width = recv_buffer[4];
						height = recv_buffer[5];
						number_of_bytes = width * height * number_of_color_channels;
						free(image_data);
						image_data = (char*) malloc(number_of_bytes * sizeof(char));

						#if DEBUG
						printf("Received Meta-Data. Width %i, Height %i\n", width, height);
						#endif
						
						if (rc > meta_informations_size)
						{
							memcpy(image_data, (char*)recv_buffer + meta_informations_size, rc - meta_informations_size);
							position -= meta_informations_size;
						}
						else
							rc = 0;
						
						recv_buffer[0] = 0;
						recv_buffer[1] = 0;
						recv_buffer[2] = 0;
						recv_buffer[3] = 0;
					}
					// First Receive and it were no Meta Data, so it must be a Picture.
					// Copy it to image_data.
					else if (recv_cycle == 1)
						memcpy(image_data, (char*)recv_buffer, rc);
				} while (rc != 0) ;

				#if DEBUG
				/*printf("Received Picture-Data. Process Data...\n");
				SDL_Surface *surf = SDL_CreateRGBSurfaceFrom((void*)image_data, width, height, 24, width * number_of_color_channels, 0xFF0000, 0x00FF00, 0x0000FF, 0xFFFFFF);
				SDL_SaveBMP(surf, "FOO.bmp");
				SDL_FreeSurface(surf);
				surf = NULL;*/
				#endif

				for (i = 0; i < ds.num_displays; ++i)
				{
					int w = 0;
					int h = 0;
					SDL_QueryTexture(ds.display[i].texture, NULL, NULL, &w, &h);
					if (width != w || height != h)
					{
						SDL_DestroyTexture(ds.display[i].texture);
						ds.display[i].texture = SDL_CreateTexture(ds.display[i].renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, width / ds.num_displays, height);						
					}

					SDL_Rect slice = {i * width / ds.num_displays, 0, width / ds.num_displays, height};

					void *pixels;
					int pitch;
					//printf("For der Color\n");
					SDL_Color *color = malloc(4 * sizeof(int));
					if ( SDL_LockTexture(ds.display[i].texture, NULL, &pixels, &pitch) < 0 )
					{
						fprintf(stderr, "SDL_LockTexture: %s\n", SDL_GetError());
						exit(EXIT_FAILURE);
					}
					//printf("For der Forschleife\n");
					for (int row = i * width / ds.num_displays; row < (number_of_bytes - width / ds.num_displays * i); row += (width / ds.num_displays) * number_of_color_channels)
					{
						int *dst = pixels + row;
						//printf("Row: %i\n", row);
						for (int col = 0; col < width / ds.num_displays * number_of_color_channels; col += 3)
						{
							color->r = image_data[row + col + 2];
							color->g = image_data[row + col + 1];
							color->b = image_data[row + col];
							//printf("HHHUIIII\n");
							*dst++ = SDL_MapRGB(SDL_PIXELFORMAT_RGB24, color->r, color->g, color->b);//(color->r<<16) | (color->g<<8) | (color->b);
						}
					}
					SDL_UnlockTexture(ds.display[i].texture);
					
					/*if (ds.display[i].texture != NULL)
					{
						SDL_DestroyTexture(ds.display[i].texture);
					}
					ds.display[i].texture = SDL_CreateTexture(ds.display[i].renderer, SDL_PIXELFORMAT_BGR24, SDL_TEXTUREACCESS_STREAMING, width, height);						
					if(ds.display[i].texture == 0)
					{
						fprintf(stderr, "SDL_CreateTexture: %s\n", SDL_GetError());
						exit(EXIT_FAILURE);
					}*/					
					/*if (SDL_UpdateTexture(ds.display[i].texture, NULL, (void *)image_data, width * number_of_color_channels) < 0)
					{
						fprintf(stderr, "SDL_UpdateTexture: %s\n", SDL_GetError());
						exit(EXIT_FAILURE);
					}*/
					if (SDL_RenderClear(ds.display[i].renderer) < 0)
					{
						fprintf(stderr, "SDL_RenderClear: %s\n", SDL_GetError());
						exit(EXIT_FAILURE);
					}
					if (SDL_RenderCopy(ds.display[i].renderer, ds.display[i].texture, /*&slice*/NULL, NULL) < 0)
					{
						fprintf(stderr, "SDL_RenderCopy: %s\n", SDL_GetError());
						exit(EXIT_FAILURE);
					}
					SDL_RenderPresent(ds.display[i].renderer);
				}
				position = 0;
			}
		}
	}

	free(image_data);
	image_data = NULL;
	free(recv_buffer);
	recv_buffer = NULL;
	for (i = 0; i < ds.num_displays; ++i)
	{
		SDL_DestroyTexture(ds.display[i].texture);
		SDL_DestroyRenderer(ds.display[i].renderer);
		SDL_DestroyWindow(ds.display[i].window);
		ds.display[i].texture = NULL;
		ds.display[i].renderer = NULL;
		ds.display[i].window = NULL;
	}
	SDL_free(ds.display);
	SDLNet_TCP_Close(client_socket);
	SDLNet_TCP_Close(server_socket);
	SDLNet_FreeSocketSet(socket_set);
	socket_set = NULL;
	SDLNet_Quit();
	SDL_Quit();
	return EXIT_SUCCESS;
}
