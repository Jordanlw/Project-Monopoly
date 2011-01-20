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
		refreshBoard(board);
		createBoard(&board,amntProperties,requirement,properties,players);
		
		SDL_BlitSurface(board,NULL,screen,NULL);
		SDL_Flip(screen);
		//Test whether user wants to exit.
		SDL_Event event;
		SDL_PollEvent(&event,amntProperties,requirement);
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

static void createBoard(SDL_Surface **board,
						int amntProperties,
						SDL_Rect requirement,
						struct property **properties,
						struct player **players)
{
	int i = 0;
	int corners = 0;
	//0 = x-x,1 = x-y,2 = x+x,3 = x-y
	int mode = 0;
	SDL_Rect position;
	position.x = 0;
	position.y = 0;
	SDL_Rect cord;
	cord.x = 0;
	cord.y = 0;
	for(i = 0;i < amntProperties;i++)
	{
		if((properties[i])->type == 1)
		{
			corners++;
		}
		if(corners == 1)
		{
			position.x = requirement.w;
			position.y = requirement.h;
		}
		SDL_Surface property;
		generateProperty(property,amntProperty,properties[i],players,requirement);	
	}
}

static void generateProperty(SDL_Surface *property,
							 int amntProperty,
							 struct property *properties,
							 struct player **players,
							 SDL_Rect requirement)
{
	*property = SDL_CreateRGBSurface(0,requirement.w,requirement.h,32,0,0,0,0);
	SDL_Rect offset;
	offset.y = 5;
	SDL_Surface text = 0;
	TTF_Font *font = TTF_OpenFont("/usr/share/fonts/TTF/mensch.ttf",12);
	SDL_Color fontColor = {0,0,0};
	int i = 0;
	int amnt = 4;
	for(i = 0;i < amnt;i++)
	{
		if(i == 0)
		{
			text = TTF_RenderText_Solid(font,properties->name,textcolor);
		}
		if(i == 1)
		{
			if(properties->owner > -1)
			{
				text = TTF_RenderText_Solid(font,(players[properties->owner])->id,textcolor);
			}
			else
			{
				text = TTF_RenderText_Solid(font,"No owner",textcolor);
			}
		}
		if(i == 2)
		{
			char *array = malloc(100);
			itoa(properties->hotels,array,10);
			text = TTF_RenderText_Solid(font,array,textcolor);
			free(array);
		}
		if(i == 3)
		{
			char *array = malloc(100);
			itoa(properties->value,array,10);
			text = TTF_RenderText_Solid(font,array,textcolor);
			free(array);
		}	
		offset.x = (requirement.w - text.w) / 2;
		if(i >= 1)
		{
			offset.y += text.h + 5;
		}
		SDL_BlitSurface(text,NULL,property,&offset);
	}		
}
static void refreshBoard(SDL_Surface *board)
{
	SDL_FillRect(board,NULL,SDL_MapRGB(board->format,255,255,255));
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
	
	
