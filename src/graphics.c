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
		puts("ERROR: error from graphicalInit()");
		quit = 1;
	}
	TTF_Font *font = TTF_OpenFont("/usr/share/fonts/TTF/mensch.ttf",12);
	SDL_Color fontColor = {0,0,0};
	SDL_Surface *board;
	SDL_Rect requirement;
	SDL_Rect boardSize;
	getReq(&requirement,font,fontColor);
	setBoardSize(properties,amntProperties,&board,requirement,&boardSize);

	while(!quit)
	{
		refreshBoard(board);
		createBoard(&board,amntProperties,requirement,properties,players,boardSize);
		
		SDL_BlitSurface(board,NULL,screen,NULL);
		SDL_Flip(screen);
		//Test whether user wants to exit.
		SDL_Event event;
		SDL_PollEvent(&event);
		if(event.type == SDL_QUIT)
		{
			quit = 1;
			*isRunning = 0;
		}
		SDL_Delay((1/30)*1000);
	}
	SDL_FreeSurface(board);
	TTF_Quit();
	SDL_Quit();
	pthread_exit(NULL);
	
}

static void createBoard(SDL_Surface **board,
						int amntProperties,
						SDL_Rect requirement,
						struct property **properties,
						struct player **players,
						SDL_Rect boardSize)
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
		SDL_Surface *property;
		generateProperty(&property,amntProperties,properties[i],players,requirement);
		cord.x = boardSize.w - position.x;
		cord.y = boardSize.h - position.y;		
		SDL_BlitSurface(property,NULL,*board,&cord);
		position.x += requirement.w;
		//DEBUG
		/*printf("prop.w %d,prop.h %d,cord.x %d,cord.y %d,position.x %d,position.y %d,board.w %d,board.h %d\n",
		property->w,property->h,cord.x,cord.y,position.x,position.y,boardSize.w,boardSize.h);*/
		
		SDL_FreeSurface(property);	
	}
}

static void generateProperty(SDL_Surface **property,
							 int amntProperty,
							 struct property *properties,
							 struct player **players,
							 SDL_Rect requirement)
{
	*property = SDL_CreateRGBSurface(0,requirement.w,requirement.h,32,0,0,0,0);
	SDL_FillRect(*property,NULL,SDL_MapRGB((*property)->format,255,255,255));
	SDL_Rect offset;
	offset.y = 5;
	SDL_Surface *text = 0;
	static int first = 1;
	static TTF_Font *font;
	if(first)
	{
		font = TTF_OpenFont("/usr/share/fonts/TTF/mensch.ttf",12);
		if(!font)
		{
			puts("ERROR: font pointer is null, in generateProperty()");
		}
		first = 0;
	}
		
	SDL_Color fontColor = {0,0,0};
	int i = 0;
	int amnt = 4;
	for(i = 0;i < amnt;i++)
	{
		if(i == 0)
		{
			text = TTF_RenderText_Solid(font,properties->name,fontColor);
		}
		if(i == 1)
		{
			if(properties->owner > -1)
			{
				text = TTF_RenderText_Solid(font,(players[properties->owner])->id,fontColor);
			}
			else
			{
				text = TTF_RenderText_Solid(font,"No owner",fontColor);
			}
		}
		if(i == 2)
		{
			char *array = malloc(100);
			char *full = malloc(100);
			sprintf(array,"%d",properties->hotels);
			sprintf(full,"Hotels: ");
			strcat(full,array);
			text = TTF_RenderText_Solid(font,full,fontColor);
			free(array);
			free(full);
		}
		if(i == 3)
		{
			char *array = malloc(100);
			char *full = malloc(100);
			sprintf(array,"%d",properties->value);
			sprintf(full,"Price: ");
			strcat(full,array);
			text = TTF_RenderText_Solid(font,full,fontColor);
			free(array);
			free(full);
		}	
		offset.x = (requirement.w - text->w) / 2;
		if(i >= 1)
		{
			offset.y += text->h + 5;
		}
		SDL_BlitSurface(text,NULL,*property,&offset);
		SDL_FreeSurface(text);
	}		
}
static void refreshBoard(SDL_Surface *board)
{
	SDL_FillRect(board,NULL,SDL_MapRGB(board->format,255,255,255));
}
static void setBoardSize(struct property **properties,
						 int amntProperties,
						 SDL_Surface **board,
						 SDL_Rect requirement,
						 SDL_Rect *boardSize)
{
	int corners = 0;
	int i = 0;
	int prop = 1;
	int w = 0;
	int h = 0;
	for(i = 0,prop = 1;i < amntProperties;i++,prop++)
	{
		if((properties[i])->type == 1)
		{
			corners++;
		}
		if(corners == 1 || corners == 3)
		{
			if(prop > w)
			{
				w = prop;
			}
			prop = 1;
		}
		if(corners == 2 || corners || 4)
		{
			if(prop > h)
			{
				h = prop;
			}
			prop = 1;
		}
	}
	boardSize->w = w * requirement.w + (requirement.h * 2);
	boardSize->h = h * requirement.h + (requirement.w * 2);
	*board = SDL_CreateRGBSurface(0,boardSize->w,boardSize->h,32,0,0,0,0);	
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
	
	
