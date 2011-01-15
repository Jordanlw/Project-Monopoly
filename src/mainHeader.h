#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "structs.h"

#define SIDES 4

int setupTmpPropertyNames(struct property **,int );
int rollDice(FILE *,int,int);
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
int gameLoop(struct property **,struct player **,int ,int ,int *,FILE *);
char queryPlayer(char *);
void actOnAction(struct property **,struct player **,int ,int ,int *,int,int,FILE *,int *);
void playerResign(int ,struct player *,struct property **,int );
int takeTurn(struct property **,struct player **,int,FILE *,int *,int);
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


#include "propertyStruct.c"
