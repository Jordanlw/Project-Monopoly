void graphicalLoop(struct property **properties,
				   struct player **players,
				   int amntProperties)
{
	static int i = 0;
	SDL_Surface gameBoard = SDL_CreateRGBSurface(0,1000,800,32,0,0,0,0);
	if(!i)
	{
		SDL_Init(SDL_INIT_EVERYTHING);
		TTF_Init();
		SDL_Surface *screen = SDL_SetVideoMode(1000,800, 32, SDL_SWSURFACE);
		i = 1;
	}
	updateBoard(&gameBoard,players,properties,amntProperties);
}

static void updateBoard(SDL_Surface *gameBoard,
			struct player **players,
			struct property **properties,
			int amntProperties);
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
		textSur = TTF_RenderText_Solid(font,(properties[i])->id,textcolor);
		SDL_Rect textPos,oldTextPos;
		textPos.x = textSur.w / 2;
		textPos.y = 0;
		SDL_BlitSurface(textSur,NULL,property,&textPos);
		
		//Blits the owner's name
		equalSdlRects(&textPos,&oldTextPos);
		textPos.y = textSur.h + oldTextPos.y;
		SDL_FreeSurface(textSur);
		textSur = TTF_RenderText_Solid(font,(players[(propertie[i])->owner])->name,textcolor);
		textPos.x = textSur.w / 2;
		SDL_BlitSurface(textSur,NULL,property,&textPos);
		
		//TODO: blit amount of hotles & price
		
		//Blit property onto gameboard
		SDL_BlitSurface(property,NULL,gameBoard,NULL);
	}
}
static void equalSdlRects(SDL_Rect *from,SDL_Rect *to)
{
	(*to).x = (*from).x;
	(*to).y = (*from).y;
	(*to).w = (*from).w;
	(*to).h = (*from).h;
}
