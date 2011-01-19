static int graphicalInit(SDL_Surface **screen)
{
	if(SDL_Init(SDL_INIT_EVERYTHING) ||	TTF_Init())
	{
		return 1;
	}
	*screen = SDL_SetVideoMode(1000,1000,32,SDL_SWSURFACE);
	if(!screen)
	{
		return 1;
	}
	return 0;
}
int graphicalMain(struct property **properties,
				   struct player **players,
				   int amntProperties)
{
	SDL_Surface *screen;
	if(graphicalInit(&screen))
	{
		return 1;
	}
	TTF_Font *font = TTF_OpenFont("/usr/share/fonts/TTF/mensch.ttf",12);
	SDL_Color fontColor = {255,255,255};
	SDL_Rect requirement;
	getReq(&requirement,font,fontColor);

	
	//Create game board.
	SDL_Surface *board;
}
static void getReq(*requirement,font,fontColor)
{
	//Find needed dimensions of each property surface.
	SDL_Surface *test = TTF_RenderText_Solid(font,"test 123 test 123",fontColor);
	//4 - Amount of surfaces applied, 4 * 5 - padding of 5, between surfaces.
	requirement->h = SDL_Surface->h * 4 + 4 * 5;
	// + 6 - padding on the sides.
	requirement->w = SDL_Surface->w + 6;
	SDL_FreeSurface(test);
}
	
	
