#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <pthread.h>
#include "structs.h"

#define SIDES 4

int setupTmpPropertyNames(struct property **,int );
int rollDice(int,int);
int amountOfLines(FILE *,int *);
int createArrayOfPtrs(int ,void ***);
int makeArryOfPropertyPtrs(int ,struct property **);
int FillArryPropertyData(struct property **,int ,FILE *);
int splitBuffer(char *,unsigned int *,char **);
int bufferPropertyFile(FILE *,char **,int );
int fillPropertyStruct(struct property *,unsigned int ,char *);
int allocPlayerArray(struct player ***,int );
int setupPlayers(struct player ***,int *);
int setupPlayerStruct(struct player **,unsigned int ,char *,int );
void setFourCorners(int ,int *);
int gameLoop(struct property **,struct player **,int ,int ,int *,volatile int *);
char queryPlayer(char *);
void actOnAction(struct property **,struct player **,int ,int ,int *,int,int,int *);
void playerResign(int ,struct player *,struct property **,int );
int takeTurn(struct property **,struct player **,int,int *,int);
void manageHotels(int ,struct player *,struct property **,int );
void status(struct property **,struct player **,int ,int ,int );
void houseStatus(int,int ,struct player **,struct property **,int,void (*)(int *));
void mortgage(struct property **,struct player **,int ,int);
void addCorners(int ,int *,struct property ***);
void trade(int ,struct player **,struct property **,int ,int );
void parseInput(int ,int *,int *,char *,int *,char *,char *);
void updateStruct(struct property **,struct player **,int ,int ,int );
void mortgagePrice(int *);
void bankruptcy(struct property **,struct player **,int ,int ,int ,int *,int *);
int needMore(void **,int *);
int kbhit(void);
void auctionHouse(int ,struct player **,struct property **,int ,int );
void *graphicalMain(void *);
static int graphicalInit(SDL_Surface **);
static void setBoardSize(struct property **,int ,SDL_Surface **,SDL_Rect,SDL_Rect *);
static void getReq(SDL_Rect *,TTF_Font *,SDL_Color );
static void refreshBoard(SDL_Surface *);
static void createBoard(SDL_Surface **,int ,SDL_Rect ,struct property **,struct player **,SDL_Rect);
static void generateProperty(SDL_Surface **,int ,struct property *,struct player **,SDL_Rect );
#include "propertyStruct.c"
#include "playerStruct.c"
#include "multiUseStruct.c"
#include "gameLoop.c"
#include "trade.c"
#include "auction.c"
#include "mortgage.c"
#include "resign.c"
#include "hotels.c"
#include "status.c"
#include "bankruptcy.c"
#include "graphics.c"
