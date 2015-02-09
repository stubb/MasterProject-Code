#include <iostream>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

#define DEBUG 0

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	UDPsocket sd;
	IPaddress srvadd;
	UDPpacket *p;
	int rc = 0;
	int numberOfRenderingClients = 1;

	VideoCapture vid(argv[1]);
	Mat img;

	if (!vid.isOpened())
	{
		cout << "Couldn't open Video." << endl;
		return 1;
	}
	
	/* Initialize SDL_net */
	if (SDLNet_Init() < 0)
	{
		fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}
	
	/* Open a socket on random port */
	if (!(sd = SDLNet_UDP_Open(0)))
	{
		fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}
	
	/* Resolve server name */
	if (SDLNet_ResolveHost(&srvadd, "localhost", 2000) == -1)
	{
		fprintf(stderr, "SDLNet_ResolveHost(%s %d): %s\n", argv[1], 
atoi(argv[2]), SDLNet_GetError());
		exit(EXIT_FAILURE);
	}
	
	/* Allocate memory for the packet */
	if (!(p = SDLNet_AllocPacket(9999999)))
	{
		fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

    img = imread(argv[1], CV_LOAD_IMAGE_COLOR);
	
	// send meta data
	int meta_data[2] = {10, 10};//{(int) vid.get(CV_CAP_PROP_FRAME_WIDTH) / numberOfRenderingClients, (int) vid.get(CV_CAP_PROP_FRAME_HEIGHT)};
	cout << meta_data[0] << " xXx " << meta_data[1] << endl;
	Uint8* md_pointer = (Uint8*) &meta_data;
	p->data = md_pointer;
	p->address.host = srvadd.host;
	p->address.port = srvadd.port;
	p->len = sizeof(meta_data) + 1;
	printf("meta p len: %d\n", p->len);
	rc = SDLNet_UDP_Send(sd, -1, p);
	if(rc == 0) 
	{
		printf("Sending metadata failed with code %i.\n", rc);
		return 1;
	}
	
	printf("img/img.data %d %d\n", (int) sizeof(img), (int) sizeof(img.data));
	//send image to all rendering clients
	Uint8* img_pointer = (Uint8*) &img.data;
	p->data = img_pointer;
	p->len = (img.total() * img.elemSize()) + 1;
	
								printf("\tmData: %d\n", (int) img.data[0]);
				printf("\tmData: %d\n", (int) img.data[1]);
				printf("\tmData: %d\n", (int) img.data[2]);
				printf("\tmData: %d\n", (int) img.data[3]);
				printf("\tmData: %d\n", (int) img.data[4]);
				printf("\tmData: %d\n", (int) img.data[5]);
				printf("\tmData: %d\n", (int) img.data[6]);
				printf("\tmData: %d\n", (int) img.data[7]);
				printf("\tmData: %d\n", (int) img.data[8]);
				printf("\tmData: %d\n", (int) img.data[9]);
				printf("\tmData: %d\n", (int) img.data[10]);
				
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
			
///	for (;;)
//	{
		for (int i = 0; i < numberOfRenderingClients; ++i)
		{

			//printf("p->len: %d\n", p->len);
			rc = SDLNet_UDP_Send(sd, -1, p);
			if (rc == 0)
			{
				printf("SDLNet_UDP_Send: %s\n", SDLNet_GetError());
				return 1;
			}
			else
			{
				//printf("p len: %d\n", p->len);
			}
		}
		waitKey(42);
//	}
}
