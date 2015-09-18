#ifndef __RENDERING_CLIENT_H__
#define __RENDERING_CLIENT_H__

/*	=========================================	*/
/*					STRUCTS						*/
/*	=========================================	*/

/*	Struct to hold Object bound to Display. */
struct DisplaySetup {
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
};

/*	Struct to hold advanced Display Infos.	*/
struct DisplayPropertys {
	int num_displays;
	int xmin, xmax;
	int ymin, ymax;
	int width, height;
	struct DisplaySetup *display;
};

/*	=========================================	*/
/*					FUNCTIONS					*/
/*	=========================================	*/
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
	}
	return 0;
}

/*	=========================================	*/
/*					GLOBALS						*/
/*	=========================================	*/
static struct DisplayPropertys dp;

#endif