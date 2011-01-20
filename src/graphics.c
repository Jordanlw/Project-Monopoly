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
void *graphicalMain(void *dataArg)
{
	void **data = dataArg;
	struct property **properties = (struct property **)data[0];
	struct player **players = (struct player **)data[1];
	int *tmp = (int *)data[2];
	int amntProperties = *tmp;
	int *isRunning = (int *)data[3];
	int quit = 0;
	*isRunning = 1;
	
	SDL_Surface *screen;
	if(graphicalInit(&screen))
	{
		quit = 1;
	}
	TTF_Font *font = TTF_OpenFont("/usr/share/fonts/TTF/mensch.ttf",12);
	SDL_Color fontColor = {0,0,0};
	SDL_Surface *board;
	SDL_Rect requirement;
	getReq(&requirement,font,fontColor);
	setBoardSize(properties,amntProperties,&board,requirement);

	while(!quit)
	{
		SDL_BlitSurface(board,NULL,screen,NULL);
		SDL_Flip(screen);
		SDL_Event event;
		SDL_PollEvent(&event);
		if(event.type == SDL_QUIT)
		{
			quit = 1;
			*isRunning = 0;
		}
		SDL_Delay((1/15)*1000);
	}
	SDL_FreeSurface(board);
	SDL_Quit();
	TTF_Quit();
	pthread_exit(NULL);
	
}

static void setBoardSize(struct property **properties,
						 int amntProperties,
						 SDL_Surface **board,
						 SDL_Rect requirement)
{
	int corners = 0;
	int i = 0;
	int prop = 0;
	int w = 0;
	int h = 0;
	for(i = 0,prop = 0;i < amntProperties;i++,prop++)
	{
		if((properties[i])->type == 1)
		{
			corners++;
		}
		if(corners == 1 || corners == 3)
		{
			if(i-prop+1 > w)
			{
				w = i-prop+1;
			}
			prop = 0;
		}
		if(corners == 2 || corners || 4)
		{
			if(i-prop+1 > h)
			{
				h = i-prop+1;
			}
			prop = 0;
		}
	}
	SDL_Rect boardRect;
	boardRect.w = w * requirement.w + requirement.h * 2;
	boardRect.h = h * requirement.h + requirement.w * 2;
	*board = SDL_CreateRGBSurface(0,boardRect.w,boardRect.h,32,0,0,0,0);
	SDL_FillRect(*board,NULL,SDL_MapRGB((*board)->format,255,255,255));	
}

static void getReq(SDL_Rect *requirement,TTF_Font *font,SDL_Color fontColor)
{
	//Find needed dimensions of each property surface.
	SDL_Surface *test = TTF_RenderText_Solid(font,"test 123 test 123",fontColor);
	//4 - Amount of surfaces applied, 4 * 5 - padding of 5, between surfaces.
	requirement->h = test->h * 4 + 4 * 5;
	// + 6 - padding on the sides.
	requirement->w = test->w + 6;
	SDL_FreeSurface(test);
}
	
	
