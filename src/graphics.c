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
	SDL_Rect position;
	position.x = 0;
	position.y = 0;
	SDL_Rect cord;
	cord.x = 0;
	cord.y = 0;
	int first = 1;
	int notNow = 0;
	
	for(i = 0;i < amntProperties + SIDES;i++,notNow = 0)
	{
		if((properties[i])->type == 1)
		{
			corners++;
		}
		if(corners == 1 && first == 1)
		{
			position.x = requirement.w * 2;
			position.y = requirement.h;
			first = 0;
			notNow = 1;
		}
		if(corners == 1 && first == 0 && notNow == 0)
		{
			position.x += requirement.w;
		}
		if(corners == 2 && first == 1)
		{
			position.x += requirement.w;
			position.y += requirement.h;
			first = 0;
			notNow = 1;
		}
		if(corners == 2 && first == 0 && notNow == 0)
		{
			position.y += requirement.h;
		}
		if(corners == 3 && first == 1)
		{
			position.x -= requirement.w;
			position.y += requirement.h;
			first = 0;
			notNow = 1;
		}
		if(corners == 3 && first == 0 && notNow == 0)
		{
			position.x -= requirement.w;
		}
		if(corners == 4 && first == 1)
		{
			position.x = requirement.w;
			position.y -= requirement.h;
			first = 0;
			notNow = 1;
		}
		if(corners == 4 && first == 0 && notNow == 0)
		{
			position.y -= requirement.h;
		}
		if(i < amntProperties + SIDES - 1)
		{
			if((properties[i+1])->type == 1)
			{
				first = 1;
			}
		}
		SDL_Surface *property;
		generateProperty(&property,amntProperties,properties[i],players,requirement);
			
		cord.x = boardSize.w - position.x;
		cord.y = boardSize.h - position.y;		
		SDL_BlitSurface(property,NULL,*board,&cord);
		
		
		//DEBUG
		/*printf("\nReq.w %d,Req.h %d,pos.x %d,pos.y %d,corn %d\n",requirement.w,requirement.h,position.x,position.y,corners);
		printf("prop %d\n",amntProperties);*/
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
	SDL_FillRect(*property,NULL,SDL_MapRGB((*property)->format,100,100,100));
	SDL_Surface *innerProperty = SDL_CreateRGBSurface(0,requirement.w-2,requirement.h-2,32,0,0,0,0);
	SDL_FillRect(innerProperty,NULL,SDL_MapRGB(innerProperty->format,255,255,255));
	SDL_Rect innerOffset;
	innerOffset.x = 1;
	innerOffset.y = 1;
	SDL_BlitSurface(innerProperty,NULL,*property,&innerOffset);
	SDL_FreeSurface(innerProperty);
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
		//-2 is for the removal of the pixel border.
		offset.x = ((requirement.w - 2) - text->w) / 2;
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
	for(i = 0,prop = 1;i < amntProperties + SIDES;i++,prop++)
	{
		if((properties[i])->type == 1)
		{
			corners++;
			prop = 1;
		}
		if(corners == 1 || corners == 3)
		{
			if(prop > w)
			{
				w = prop;
			}
		}
		if(corners == 2 || corners == 4)
		{
			if(prop > h)
			{
				h = prop;
			}
		}
	}
	boardSize->w = w * requirement.w + (requirement.w * 2);
	boardSize->h = h * requirement.h + (requirement.h * 2);
	//DEBUG
	/*printf("Req.w %d,Req.h %d\n",requirement.w,requirement.h);
	printf("Board.w %d,Board.h %d\n",boardSize->w,boardSize->h);
	printf("w %d,h %d\n",w,h);*/
	
	*board = SDL_CreateRGBSurface(0,boardSize->w,boardSize->h,32,0,0,0,0);	
}

static void getReq(SDL_Rect *requirement,TTF_Font *font,SDL_Color fontColor)
{
	//Find needed dimensions of each property surface.
	SDL_Surface *test = TTF_RenderText_Solid(font,"test 123 test 123",fontColor);
	//4 - Amount of surfaces applied, 4 * 5 - padding of 5, between surfaces.
	//+2 on both the w & h, is for the 1pixel wide border on all sides.
	requirement->h = test->h * 4 + 4 * 5 + 2;
	// + 6 - padding on the sides.
	requirement->w = test->w + 6 + 2;
	SDL_FreeSurface(test);
}
	
	
