void graphicalLoop(struct property **properties,
				   struct player **players,
				   int amntProperties,
				   int *quit)
{
	static int i = 0;
	SDL_Surface *gameBoard = SDL_CreateRGBSurface(0,1000,800,32,0,0,0,0);
	SDL_Surface *screen;
	SDL_Event event;
	SDL_PollEvent(&event);
	if(event.type == SDL_QUIT)
	{
		*quit = 1;
	}
	if(!i)
	{
		SDL_Init(SDL_INIT_EVERYTHING);
		TTF_Init();
		screen = SDL_SetVideoMode(1000,800, 32, SDL_SWSURFACE);
		i = 1;
	}
	updateBoard(gameBoard,players,properties,amntProperties);
	SDL_BlitSurface(gameBoard,NULL,screen,NULL);
	SDL_Flip(screen);
}

static void updateBoard(SDL_Surface *gameBoard,
			struct player **players,
			struct property **properties,
			int amntProperties)
{
	int i = 0;
	for(i = 0;i < amntProperties;i++)
	{
		TTF_Font *font = NULL;
		SDL_Color textcolor = {0,0,0};
		font = TTF_OpenFont( "/usr/share/fonts/TTF/mensch.ttf", 12 );
		if(!font)
		{
			exit(1);
		}
		SDL_Surface *property = SDL_CreateRGBSurface(0,100,300,32,0,0,0,0);
		SDL_Surface *textSur = NULL;
		SDL_Surface *oldTextSur = NULL;
		SDL_Rect textRect;
		//Blits the property's name
		textSur = TTF_RenderText_Solid(font,(properties[i])->name,textcolor);
		SDL_Rect textPos,oldTextPos;
		textPos.x = textSur->w / 2;
		textPos.y = 0;
		SDL_BlitSurface(textSur,NULL,property,&textPos);
		
		//Blits the owner's name
		equalSdlRects(&textPos,&oldTextPos);
		textPos.y = textSur->h + oldTextPos.y;
		SDL_FreeSurface(textSur);
		if((properties[i])->owner == -1)
		{
			textSur = TTF_RenderText_Solid(font,"Unowned",textcolor);
		}
		else
		{
			textSur = TTF_RenderText_Solid(font,(players[(properties[i])->owner])->id,textcolor);
		}
		textPos.x = textSur->w / 2;
		SDL_BlitSurface(textSur,NULL,property,&textPos);
		
		//TODO: blit amount of hotles & price
		
		//Blit property onto gameboard
		SDL_BlitSurface(property,NULL,gameBoard,NULL);
		if(i+1 >= amntProperties)
		{
			TTF_CloseFont(font);
			SDL_FreeSurface(property);
			SDL_FreeSurface(textSur);
		}
	}
	
}
static void equalSdlRects(SDL_Rect *from,SDL_Rect *to)
{
	(*to).x = (*from).x;
	(*to).y = (*from).y;
	(*to).w = (*from).w;
	(*to).h = (*from).h;
}
