/*	DEBUG Flag.	*/
#define DEBUG 0

/*	Standard Libs.	*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <inttypes.h>
#include <stdint.h>

/*	SDL Includes.	*/
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

/*	Own Header.	*/
#include "rc.h"

int main(int argc, char *argv[])
{
	#if DEBUG
		printf("RenderingClient by KK & JB.");
	#endif

	/*	Addresses.	*/
	IPaddress ip_adress;
	int listening_port = 2000;

	/*	Sockets.	*/
	TCPsocket server_socket = NULL;
	TCPsocket client_socket = NULL;

	/*	Socketset for holding all Connections.	*/
	SDLNet_SocketSet socket_set;

	/*	Returncodes and helper Variables.	*/
	int num_used_sockets = 0;
	int returnCode = 0;
	int i;


/*	=========================================	*/
/*					SDL INIT					*/
/*	=========================================	*/

	/*	Init SDL System.	*/
	#if DEBUG
		printf("SDLNet_Init\t\t");
	#endif
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	if (SDLNet_Init() < 0)
	{
		fprintf(stderr, "\nSDLNet_Init: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}
	#if DEBUG
		printf("...successful.\n");
	#endif


	/*	Disable Mouseicon on Window.	*/
	SDL_EventState(SDL_MOUSEMOTION, SDL_DISABLE);


/*	=========================================	*/
/*				SDL_NET INIT					*/
/*	=========================================	*/

	/*	One Socket in the Set is enough for now, because we (the RC) are the Server.	*/
	socket_set = SDLNet_AllocSocketSet(1);
	if(!socket_set)
	{
		fprintf(stderr, "SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

	/*	Resolve Host Settings.	*/
	#if DEBUG
		printf("SDLNet_ResolveHost\t\t");
	#endif
	/*	NULL makes Host to self -> Socket becomes Server.	*/
	if (SDLNet_ResolveHost(&ip_adress, NULL, listening_port) == -1)
	{
		fprintf(stderr, "\nSDLNet_ResolveHost: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}
	#if DEBUG
		printf("...successful.\n");
		printf("Listening on Port: %d\n", listening_port);
	#endif



	/*	Open Socket.	*/
	#if DEBUG
		printf("SDLNet_ResolveHost\t\t");
	#endif
	server_socket = SDLNet_TCP_Open(&ip_adress);
	if(!server_socket)
	{
		fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}
	/*	Wait for Client and try to connect to it.	*/
	while (client_socket == NULL)
	{
		client_socket = SDLNet_TCP_Accept(server_socket);
		if(!client_socket)
		{
			#if DEBUG
				fprintf(stderr, "SDLNet_TCP_Accept: %s\n", SDLNet_GetError());
			#endif
			SDL_Delay (1000);
		}
	}

	num_used_sockets = SDLNet_TCP_AddSocket(socket_set, client_socket);
	if(num_used_sockets == -1)
	{
		fprintf(stderr, "SDLNet_AddSocket: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}
	#if DEBUG
		printf("Accepted and Added %i Clientsocket.\n", num_used_sockets);
	#endif


/*	=========================================	*/
/*					WINDOW INIT					*/
/*	=========================================	*/

	/*	Preventing the minimizing of our fullscreen windows when the focus is lost.	 */
	SDL_SetHint("SDL_VIDEO_MINIMIZE_ON_FOCUS_LOSS", "0");


	/*	Get the Number of connected Displays.	*/
	dp.num_displays = SDL_min(10, SDL_GetNumVideoDisplays());


	/*	Allocate Memory for our Display "Objects".	*/
	dp.display = SDL_malloc(sizeof(struct DisplaySetup) * dp.num_displays);
	if (dp.display == NULL)
	{
		fprintf(stderr, "No Displays found!\n'");
		exit(EXIT_FAILURE);
	}


	/*	Iterate over each Display and initialise it with required Parts.	*/
	for (i = 0; i < dp.num_displays; ++i)
	{
		dp.display[i].texture = NULL;

		/*	Window Name for Display. Too lazy to write some proper title
			string handling here.
		 */
		char title[] = "Window 0";
		title[sizeof(title) - 2] += (char) i;

		/*	Create Window and Renderer for Display.	*/
		#if DEBUG
			printf("Init %s for Display #%i\t\t", title, i);
		#endif
		dp.display[i].window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED_DISPLAY(i), SDL_WINDOWPOS_CENTERED_DISPLAY(i), 640, 480, SDL_WINDOW_FULLSCREEN_DESKTOP);
		dp.display[i].renderer = SDL_CreateRenderer(dp.display[i].window, -1, 0);

		/*	Check if initialisation was successful.	*/
		if (dp.display[i].window == NULL)
		{
			fprintf(stderr, "\nCouldn't init %s for Display #%i.\n", title, i);
			exit(EXIT_FAILURE);
		}
		#if DEBUG
			printf("...successful.\n");
		#endif
	}
	
	/*	Set default Color of Screens to Black.	*/
	for (i = 0; i < dp.num_displays; ++i)
	{
		SDL_SetRenderDrawColor(dp.display[i].renderer, 0, 0, 0, 0);
		if (SDL_RenderClear(dp.display[i].renderer) != 0)
		{
			fprintf(stderr, "\nCouldn't clear renderer for Display #%i: %s\n", i, SDL_GetError());
		}
		SDL_RenderPresent(dp.display[i].renderer);
	}


/*	=========================================	*/
/*					MAIN PART					*/
/*	=========================================	*/

	/*	Number of Meta Data. Must be 4 + "Actual Values", because Meta Data
		are identified by 4 successive '1337' INT Values.
	 */
	int number_of_meta_informations = 6;
	int meta_informations_size = number_of_meta_informations * sizeof(int);

	int number_of_color_channels = 3;

	/*	Receive Buffer.	Will take 1280 * 720 * 3 * 4 Bytes max.	*/
	int *recv_buffer = (int*) malloc(1280 * 720 * 3 * sizeof(int));
	/*	Buffer for Actual Image Data.	*/
	char *image_data = NULL;

	/*	Number of Bytes we expect to Receive per Transmission.	*/
	int number_of_bytes = meta_informations_size;

	/*	Position for writing into the Buffer.	*/
	int position = 0;

	/*	Width and Height of Picture we want to receive.	*/
	int width = 0;
	int height = 0;

	#if DEBUG
		printf("Waiting for Data to receive.\n");
	#endif
	while (!PollEvents())
	{
		/*	Check Network for Activity.	*/
		int numready = SDLNet_CheckSockets(socket_set, 1000);
		if(numready == -1)
		{
			fprintf(stderr, "SDLNet_CheckSockets: %s\n", SDLNet_GetError());
			/*	most of the time this is a system error, where perror might help you.	*/
			perror("SDLNet_CheckSockets");
		}
		else if(numready)
		{
			#if DEBUG
				printf("There are %d sockets with activity!\n", numready);
			#endif

			/*	Check all sockets with SDLNet_SocketReady and handle the active ones.	*/
			if(SDLNet_SocketReady(client_socket))
			{
				#if DEBUG
					printf("Starting to receive the data. Awaiting %i Bytes total.\n", number_of_bytes);
				#endif
				
				/*	Will count the Cycles it took to Receive the full Data.	*/
				int recv_cycle = 0;
				
		/*	=========================================	*/
		/*				RECEIVE THE DATA				*/
		/*	=========================================	*/

				/*	Receive the Data.	*/
				do
				{
					++recv_cycle;
					/*	Are there Bytes left to receive?	*/
					if (number_of_bytes - position > 0)
					{
						/*	Receive Data. First Cycle into the big recv_buffer, because it could be Meta Data (int).
							Second Cycle of Receive, we know its Picture Data, so put everything into image_data right away.
						 */
						if (recv_cycle < 2)
							returnCode = SDLNet_TCP_Recv(client_socket, recv_buffer + position, number_of_bytes - position);
						else
							returnCode = SDLNet_TCP_Recv(client_socket, image_data + position, number_of_bytes - position);

						/*	Socket Connection still OK?	*/
						if (returnCode < 0)
						{
							printf("SDLNet_TCP_Recv failed with Code %i %i ...exit\n", returnCode, errno);
							exit(EXIT_FAILURE);
						}

						// #if DEBUG
							// printf("Total Received: %i Bytes, Last Received: %i Bytes\n", position, returnCode);
						// #endif

						/*	Advance Position for received Bytes.	*/
						position += returnCode;
					}
					else
						returnCode = 0;

					/*	Check if we received Meta Data.	*/
					if (recv_buffer[0] == 1337 && recv_buffer[1] == 1337 && recv_buffer[2] == 1337 && recv_buffer[3] == 1337)
					{
						/*	Set some Properties with the received Meta Data.	*/
						width = recv_buffer[4];
						height = recv_buffer[5];
						number_of_bytes = width * height * number_of_color_channels;
						
						/*	Free Image Buffer and Allocate new Space for upcoming Picture.	*/
						free(image_data);
						image_data = (char*) malloc(number_of_bytes * sizeof(char));

						#if DEBUG
							printf("Received Meta Data. Width: %i; Height: %i.\n", width, height);
						#endif
						
						/*	Sometimes we already Receive Picture Data with the Meta Data Cycle (especially on UNIX),
							so check if we Received more than the expected Size of Meta Data, and write these Picture
							Data into the Image Buffer.
							Else just set the Return Code to 0 to Exit the Receiving Loop.
						 */
						if (returnCode > meta_informations_size)
						{
							memcpy(image_data, (char*)recv_buffer + meta_informations_size, returnCode - meta_informations_size);
							position -= meta_informations_size;
						}
						else
							returnCode = 0;
						
						/*	Clear Fields in the Buffer, which identify Meta Data.	*/
						recv_buffer[0] = 0;
						recv_buffer[1] = 0;
						recv_buffer[2] = 0;
						recv_buffer[3] = 0;
					}
					/*	First Receive and it were no Meta Data, so it must be a Picture.
						Copy it to image_data.
					 */
					else if (recv_cycle == 1)
						memcpy(image_data, (char*)recv_buffer, returnCode);
				} while (returnCode != 0) ;

				// #if DEBUG
					// printf("Received Picture-Data. Process Data...\n");
					// SDL_Surface *surf = SDL_CreateRGBSurfaceFrom((void*)image_data, width, height, 24, width * number_of_color_channels, 0xFF0000, 0x00FF00, 0x0000FF, 0xFFFFFF);
					// SDL_SaveBMP(surf, "FOO.bmp");
					// SDL_FreeSurface(surf);
					// surf = NULL;
				// #endif

		/*	=========================================	*/
		/*				RENDER THE DATA					*/
		/*	=========================================	*/

				/*	Current Width and Height of Texture.	*/
				int w = 0;
				int h = 0;

				/*	Width and Height of ROI of the Picture we want to put onto Texture.	*/
				int slice_width = width / dp.num_displays;
				int slice_height = height;
				
				#if DEBUG
					printf("Start Rendering the Picture.\n");
				#endif
				/*	Iterate over each Display.	*/
				for (i = 0; i < dp.num_displays; ++i)
				{
					/*	Get current Width and Height of Texture.	*/
					SDL_QueryTexture(dp.display[i].texture, NULL, NULL, &w, &h);

					/*	If the new Picture has a different Resolution than our Texture,
						we need to Destroy the old one and Init a new one.
					 */
					if (width != w || height != h)
					{
						SDL_DestroyTexture(dp.display[i].texture);
						/*	BGR24 Pixelformat, because we Expect OpenCV Style Pixel Data.	*/
						dp.display[i].texture = SDL_CreateTexture(dp.display[i].renderer, SDL_PIXELFORMAT_BGR24, SDL_TEXTUREACCESS_STREAMING, slice_width, slice_height);
					}

					/*	Pixel Data of Texture we want to manipulate.	*/
					char *pixels;
					
					/*	Offset we need to Jump in the Array, because the received Picture Data are 1D,
						and we fill virtually a 2D Array.
					 */
					int pitch;

					/*	Lock Texture so we can maniulate the Data.	*/
					if (SDL_LockTexture(dp.display[i].texture, NULL, (void *)&pixels, &pitch) < 0)
					{
						fprintf(stderr, "SDL_LockTexture: %s\n", SDL_GetError());
						exit(EXIT_FAILURE);	
					}
					/*	Calculate Offset of Data between 2D and 1D Array Representation.	*/
					pitch = width * number_of_color_channels;

					/*	Seperate Index for Texture Data, because these are sequential.	*/
					int index = 0;
					/*	Start at slice * color_channels * index_of_display; Stop at total byte values; increment by pitch.	*/
					for (int row = slice_width * i * number_of_color_channels; row < number_of_bytes; row += pitch)
						/*	Start at Rowstart; Stop	at End of Slice * color_channels; Increment by 1 Byte. */
						for (int col = row; col < row + slice_width * number_of_color_channels; ++col, ++index)
							pixels[index] = image_data[col];

					/*	Unlock Texture, because we are done here.	*/
					SDL_UnlockTexture(dp.display[i].texture);

					/*	Update the Renderers.	*/
					if (SDL_RenderClear(dp.display[i].renderer) < 0)
					{
						fprintf(stderr, "SDL_RenderClear: %s\n", SDL_GetError());
						exit(EXIT_FAILURE);
					}
					if (SDL_RenderCopy(dp.display[i].renderer, dp.display[i].texture, NULL, NULL) < 0)
					{
						fprintf(stderr, "SDL_RenderCopy: %s\n", SDL_GetError());
						exit(EXIT_FAILURE);
					}
					SDL_RenderPresent(dp.display[i].renderer);
				}
				#if DEBUG
					printf("Finished Rendering the Picture.\n");
				#endif

				/*	We are done Receiving and Rendering. Reset Counter for receiving Data.	*/
				position = 0;
			}
		}
	}
	
/*	=========================================	*/
/*					CLEANUP						*/
/*	=========================================	*/

	#if DEBUG
		printf("Freeing Buffers\t\t");
	#endif
	/*	Delete and Free all Buffers.	*/
	free(image_data);
	image_data = NULL;
	free(recv_buffer);
	recv_buffer = NULL;
	
	#if DEBUG
		printf("...successful.\n");
		printf("Freeing Displays\t\t");
	#endif

	/*	Delete and Free each Display.	*/
	for (i = 0; i < dp.num_displays; ++i)
	{
		SDL_DestroyTexture(dp.display[i].texture);
		SDL_DestroyRenderer(dp.display[i].renderer);
		SDL_DestroyWindow(dp.display[i].window);
		dp.display[i].texture = NULL;
		dp.display[i].renderer = NULL;
		dp.display[i].window = NULL;
	}
	SDL_free(dp.display);

	#if DEBUG
		printf("...successful.\n");
		printf("Quitting Program.\n"); 
	#endif

	/*	Delete and Free Sockets and call Cleanup of SDL Framework.	*/
	SDLNet_TCP_Close(client_socket);
	SDLNet_TCP_Close(server_socket);
	SDLNet_FreeSocketSet(socket_set);
	socket_set = NULL;
	SDLNet_Quit();
	SDL_Quit();
	
	/*	Exit Program Successfully.	*/
	return EXIT_SUCCESS;
}
