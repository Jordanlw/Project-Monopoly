#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SIDES 4

struct property
{
	char *name;
	unsigned int value;
	int owner;
	unsigned int type;
	int mortgaged;
	int hotels;
};

struct player
{
	char *id;
	int money;
	unsigned int position;
	int playing;
	int property;
};

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

int main(void)
{	
	int linesInPropertyFile = 0;
	struct property **arrayForProperties = 0;
	struct player **arrayForPlayers = 0;
	int totalLinesInPropFile = 0;
	int amntPlayers = 0;
	int *cornerPositions = malloc(sizeof(int) * SIDES);
	
	//Open /dev/urandom for rollDice
	FILE *urandom = fopen("/dev/urandom","rb");
	FILE *propertyFile = fopen("/home/jordan/Documents/Programming/Monopoly Project/properties","rb");
	if(propertyFile == NULL || urandom == NULL)
	{
		puts("ERROR: error in opening file(s)");
		return 1;
	}
	linesInPropertyFile = amountOfLines(propertyFile,&totalLinesInPropFile);
	/*//DEBUG
	printf("%d is contained within \"linesInPropertyFile\"\n",linesInPropertyFile);*/
	if(createArrayOfPtrs(linesInPropertyFile,(void ***)&arrayForProperties))
	{
		puts("ERROR: error from createArrayOfPointers() :: non-zero returned");
		return 1;
	}
	/*//DEBUG
	printf("Outside Pointer: %p\n",arrayForProperties);*/
	
	if(makeArryOfPropertyPtrs(linesInPropertyFile,arrayForProperties))
	{
		puts("ERROR: error from createArrayOfPointersForProperties() :: non-zero returned");
		return 1;
	}
	if(FillArryPropertyData(arrayForProperties,linesInPropertyFile,propertyFile))
	{
		puts("ERROR: error from FillArryPropertyData() :: non-zero returned");
	}
	if(setupTmpPropertyNames(arrayForProperties,linesInPropertyFile))
	{
		puts("ERROR: error from setupTmpPropertyNames() :: non-zero returned");
	}
	if(setupPlayers(&arrayForPlayers,&amntPlayers))
	{
		puts("ERROR: error from setupPlayers() :: non-zero returned");
	}
	setFourCorners(linesInPropertyFile,cornerPositions);
	addCorners(linesInPropertyFile,cornerPositions,&arrayForProperties);
	if(gameLoop(arrayForProperties,arrayForPlayers,linesInPropertyFile,amntPlayers,cornerPositions,urandom))
	{
		puts("ERROR: error from gameLoop() :: non-zero returned");
	}
	
	
	//DEBUG
	/*printf("array: %p\n",arrayForProperties);
	printf("*array: %p\n",*arrayForProperties);
	printf("Lines W/O comments: %d Total lines comments: %d\n\n",linesInPropertyFile,totalLinesInPropFile);
	int i = 0;
	for(i = 0;i < 4;i++)
	{
		printf("CORNER POSITION #%d: %d\n",i,cornerPositions[i]);
	}
	for(i = 0;i < linesInPropertyFile;i++)
	{
		printf("NAME: %s\n",(arrayForProperties[i])->name);
		printf("MONEY: %d\n",(arrayForProperties[i])->value);
	}
	puts("");
	for(i = 0;i < amntPlayers;i++)
	{
		printf("NAME: %s\n",(arrayForPlayers[i])->id);
		printf("MONEY: %d\n",(arrayForPlayers[i])->money);
	}*/
	
	//free allocated memory
	int i = 0;
	for(i = 0;i < linesInPropertyFile + SIDES;i++)
	{
		if((arrayForProperties[i])->type != 1)
		{
			free((arrayForProperties[i])->name);
		}
		free(arrayForProperties[i]);
	}
	free(arrayForProperties);
	for(i = 0;i < amntPlayers;i++)
	{
		free((arrayForPlayers[i])->id);
		free(arrayForPlayers[i]);
	}
	free(arrayForPlayers);
	
	//Close FILE stream for /dev/urandom
	fclose(urandom);
	fclose(propertyFile);
	return 0;
}

void trade(int current,struct player **players,struct property **properties,int amntProperties,int amntPlayers)
{
	puts("Do you want to give a [p]roperty or [m]oney?");
	int result = 0;
	int select = 0;
	char desired[2] = {'p','m'};
	parseInput(2,&result,NULL,desired,NULL,"[p]roperty","[m]oney");
	if(result == 1)
	{
		if((players[current])->property <= 0)
		{
			puts("You don't own any properties.");
			return;
		}
		houseStatus(amntPlayers,amntProperties,players,properties,current,NULL);
		while(1)
		{
			int range[2] = {1,amntProperties + SIDES};
			parseInput(1,NULL,&select,NULL,range,NULL,NULL);
			if((properties[select-1])->owner != current)
			{
				puts("You don't own that property.");
			}
			else
			{
				break;
			}
		}
		select--;
		puts("Do you want something in return?");
		int answer = 0;
		char options[2] = {'y','n'};
		parseInput(2,&answer,NULL,options,NULL,"[y]es","[n]o");
		if(answer == 1)
		{
			puts("Do you want [m]oney or [p]roperties?");
			int decide = 0;
			options[0] = 'm';
			options[1] = 'p';
			parseInput(2,&decide,NULL,options,NULL,"[m]oney","[p]roperties");
			if(decide)
			{
				puts("How much?");
				int amount = 0;
				parseInput(1,NULL,&amount,NULL,NULL,NULL,NULL);
				puts("From who?");
				int i = 0;
				for(i = 0;i < amntPlayers;i++)
				{
					if(i == current)
					{
						continue;
					}
					printf("#%d %s\n",i+1,(players[i])->id);
				}
				int selPla2 = 0;
				int range[2] = {1,amntPlayers};
				parseInput(1,NULL,&selPla2,NULL,range,NULL,NULL);
				selPla2--;
				printf("%s, Do you want to accept the trade?\n%s is giving property %s, in return taking $%d\n"
				,(players[selPla2])->id,(players[current])->id,(properties[select])->name,amount);
				char YaN[2] = {'y','n'};
				int	YaNResult = 0;
				parseInput(2,&YaNResult,NULL,YaN,NULL,"[y]es","[n]o");
				if(YaNResult != 1)
				{
					puts("Player did not accept.");
					return;
				}
				(players[selPla2])->money -= amount;
				(properties[select])->owner = selPla2;
			}
			else
			{
				houseStatus(amntPlayers,amntProperties,players,properties,-1,NULL);
				puts("Which property do you want?");
				int prop = 0;
				int range[2] = {1,amntProperties + SIDES};
				while(1)
				{
					parseInput(1,NULL,&prop,NULL,range,NULL,NULL);
					if((properties[prop-1])->owner != current)
					{
						break;
					}
					if((properties[prop-1])->owner == -1)
					{
						puts("That property isn't owned.");
						continue;
					}
					puts("You own that property.");	
				}
				prop--;
				printf("%s, do you want to receive this property %s, in exchange for %s?"
				,(players[(properties[prop])->owner])->id,(properties[select])->name,(properties[prop])->name);
				int answer = 0;
				char want[2] = {'y','n'};
				parseInput(2,&answer,NULL,want,NULL,"[y]es","[n]o");
				if(answer == 1)
				{
					(properties[select])->owner = (properties[prop])->owner;
					(properties[prop])->owner = current;
				}
				else
				{
					puts("Player did not accept.");
				}
			}		
		}
		else
		{
			int selPla = 0;
			while(1)
			{
				int i = 0;
				for(i = 0;i < amntPlayers;i++)
				{
					if(i == current)
					{
						continue;
					}
					printf("#%d %s\n",i+1,(players[i])->id);
				}
				puts("Who would you like to give this to?");
				int range[2] = {1,amntPlayers};
				parseInput(1,NULL,&selPla,NULL,range,NULL,NULL);
				if(selPla != current && (selPla > 0 && selPla <= amntPlayers))
				{
					selPla--;
					break;
				}
				else
				{
					puts("Please enter a number displayed.");
				}
			}
			(properties[select])->owner = selPla;
		}
	}
	else
	{
		puts("How much do you want to give?");
		int money = 0;
		while(1)
		{
			parseInput(1,NULL,&money,NULL,NULL,NULL,NULL);
			if(money > 0)
			{
				break;
			}
			else
			{
				puts("Please enter a number above zero.");
			}
		}
		if((players[current])->money < money)
		{
			puts("You don't have enough money.");
			return;
		}
		puts("Who would you like to give this to?");
		int i = 0;
		for(i = 0;i < amntPlayers;i++)
		{
			if(i == current)
			{
				continue;
			}
			printf("#%d %s\n",i+1,(players[i])->id);
		}
		int selPla = 0;
		int range[2] = {1,amntPlayers};
		parseInput(1,NULL,&selPla,NULL,range,NULL,NULL);
		selPla--;
		(players[current])->money -= money;
		(players[selPla])->money += money;
	}			
}

void parseInput(int intOrAns,int *result,int *num,char *desired,int *range,char *positive,char *negative)
{
	//intOrAns :: 1 for int, 2 for Answer
	//Answer stored in *result, 1 for yes, 0 for no
	//Parsed int stored in *num
	//*range stores, as the first element the lowest accepted value and as the second, the highest value accepted.
	//Desired informs us on what the caller wants recongised as positive response and negative,
	//the first element is the positive, the second is the negative
	//*positive/*negative store the string that will replace positive and negative when outputed.
	char retr = 0;
	if(result)
	{
		*result = 0;
	}
	if(num)
	{
		*num = 0;
	}
	if(intOrAns == 1)
	{
		while(1)
		{
			retr = getchar();
			if(retr == '\n')
			{
				*num /= 10;
				if(range)
				{
					if(*num >= range[0] && *num <= range[1])
					{
						break;
					}
					else
					{
						*num = 0;
						printf("Please enter a value above or equal to %d and below or equal to %d.\n",range[0],range[1]);
					}
				}
				else
				{
					break;
				}
			}
			if(retr != '\n')
			{
				*num += retr - '0';
				*num *= 10;
			}
		}
	}
	else if(intOrAns == 2)
	{
		while(1)
		{
			retr = getchar();
			while(1)
			{
				if(retr == '\n')
				{
					break;
				}
				char tmp = getchar();
				if(tmp == '\n')
				{
					break;
				}
			}
			if(retr == '\n')
			{
				if(positive != NULL && negative != NULL)
				{		
					printf("Please enter %c for %s and %c for %s.\n",desired[0],positive,desired[1],negative);
				}
				else
				{
					printf("Please enter %c for positive and %c for negative.\n",desired[0],desired[1]);
				}
				continue;
			}
			if(retr == desired[0] || retr == (desired[0] + 'A' - 'a'))
			{
				*result = 1;
				break;
			}
			else if(retr == desired[1] || retr == (desired[1] + 'A' - 'a'))
			{
				*result = 0;
				break;
			}
			else
			{
				if(positive != NULL && negative != NULL)
				{		
					printf("Please enter %c for %s and %c for %s.\n",desired[0],positive,desired[1],negative);
				}
				else
				{
					printf("Please enter %c for positive and %c for negative.\n",desired[0],desired[1]);
				}
			}
		}
	}
}

int kbhit()
{
	struct timeval tv;
	fd_set fds;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO(&fds);
	FD_SET(0,&fds);
	select(0+1,&fds,NULL,NULL,&tv);
	return FD_ISSET(0,&fds);
}

void auctionHouse(int current,struct player **players,struct property **properties,int amntProperties,int amntPlayers)
{
	if(!(players[current])->property)
	{
		puts("You don't own any properties, can't auction anything.");
		return;
	}
	houseStatus(amntPlayers,amntProperties,players,properties,current,NULL);
	puts("\nWhat property would you like to auction?");
	int propNum = 0;
	int range[2] = {1,amntProperties + SIDES};
	while(1)
	{
		parseInput(1,NULL,&propNum,NULL,range,NULL,NULL);
		if((properties[propNum-1])->owner == current)
		{
			break;
		}
		else
		{
			puts("You don't own that property.");
		}
	}
	propNum--;
	puts("What value would you like to start bidding at?");
	int propPrice = 0;
	parseInput(1,NULL,&propPrice,NULL,NULL,NULL,NULL);
	int seconds = 10;
	int currentBid = 0;
	int display = 1;
	while(1)
	{
		int i = 0;
		if(display)
		{
			printf("Current bid $%d\n",propPrice);
			for(i = 0;i < amntPlayers;i++)
			{
				printf("#%d %s, ",i+1,(players[i])->id);
			}
			puts("");
		}
		display = 0;
		if(kbhit())
		{
			display = 1;
			int selPla = 0;
			parseInput(1,NULL,&selPla,NULL,NULL,NULL,NULL);
			selPla--;
			if(selPla >= 0 && selPla < amntPlayers)
			{
				if((players[selPla])->money >= propPrice)
				{
					currentBid = selPla;
					propPrice += propPrice * .15;
					seconds = 10;
				}
				else
				{
					puts("You don't have enough funds for the property.");
				}
			}
			else
			{
				puts("Please enter an ID, of one of the listed players.");
			}

		}
		sleep(1);
		seconds--;
		printf("You have %d seconds left.\n",seconds);
		if(seconds <= 0)
		{
			printf("The winner is %s at $%d\n",(players[currentBid])->id,propPrice);
			(properties[propNum])->owner = currentBid;
			(players[currentBid])->money -= propPrice;
			break;
		}
	}
}
void addCorners(int linesInPropertyFile,int *cornerPositions,struct property ***arrayForProperties)
{ 
	//Create alternative array and fill with data
	void *tmp3 = 0;
	tmp3 = (void *)realloc(*arrayForProperties,(linesInPropertyFile + SIDES) * sizeof(struct property *));
	int i = 0;
	if(tmp3 == 0)
	{
		puts("ERROR: error in addCorners :: null contained in tmp3");
	}
	else
	{
		*arrayForProperties = tmp3;
	}
	for(i = 0;i < SIDES;i++)
	{
		(*arrayForProperties)[linesInPropertyFile + i] = malloc(sizeof(struct property));
	}	
	struct property tmp1;
	struct property tmp2;
	
	//Add in corners
	int cornDone = 0;
	for(;cornDone < SIDES;cornDone++)
	{
		int next = (cornerPositions[cornDone] - 1);
		tmp1 = *((*arrayForProperties)[next]);
		((*arrayForProperties)[next])->name = "Corner position";
		((*arrayForProperties)[next])->value = 0;
		((*arrayForProperties)[next])->owner = -1;
		((*arrayForProperties)[next])->type = 1;
		((*arrayForProperties)[next])->mortgaged = 0;
		((*arrayForProperties)[next])->hotels = 0;
		next++;
		for(;next < linesInPropertyFile + SIDES;next++)
		{
			tmp2 = *((*arrayForProperties)[next]);
			*((*arrayForProperties)[next]) = tmp1;
			tmp1 = tmp2;
		}
	}	
}
	
int gameLoop(struct property **properties,
		     struct player **players,
		 	 int amntProperties,
		 	 int amntPlayers,
			 int *corners,
			 FILE *urandom)
{
	int currentPlayer = 0;
	int i = 0;
	int doubles[1] = {0};
	
	if(amntPlayers == 0)
	{
		puts("# You didn't enter in any players, exiting #");
		exit(0);
	}
	for(currentPlayer = 0;;currentPlayer++)
	{
		if(currentPlayer >= amntPlayers)
		{
			currentPlayer = 0;
		}
		if((players[currentPlayer])->playing == 0)
		{
			i++;
			if(i >= amntPlayers)
			{
				break;
			}
			continue;
		}
		if(*doubles == 1)
		{
			*doubles = 0;
			if(currentPlayer == 0)
			{
				currentPlayer = amntPlayers-1;
			}
			else
			{
				currentPlayer--;
			}
		}
		updateStruct(properties,players,amntProperties,amntPlayers,currentPlayer);
		i = 0;
		int action = queryPlayer((players[currentPlayer])->id);
		actOnAction(properties,players,amntProperties,amntPlayers,corners,action,currentPlayer,urandom,doubles);
	}
	return 0;
}

void updateStruct(struct property **properties,struct player **players,int amntProperties,int amntPlayers,int currentPlayer)
{
	int i = 0;
	for(i = 0;i < amntProperties + SIDES;i++)
	{
		if((properties[i])->owner == currentPlayer)
		{
			(players[currentPlayer])->property++;
		}
	}
}

void actOnAction(struct property **properties,
				 struct player **players,
				 int amntProperties,
				 int amntPlayers,
				 int *corners,
				 int action,
				 int current,
				 FILE *urandom,
				 int *doubles)
{
	switch (action)
	{
		case 1 :
			playerResign(current,players[current],properties,amntProperties);
			break;
		case 2:
			mortgage(properties,players,amntProperties,current);
			*doubles = 1;
			break;
		case 3 :
			manageHotels(current,players[current],properties,amntProperties);
			*doubles = 1;
			break;
		case 4:
			trade(current,players,properties,amntProperties,amntPlayers);
			*doubles = 1;
			break;
		case 5 :
			auctionHouse(current,players,properties,amntProperties,amntPlayers);
			*doubles = 1;
			break;
		case 6 :
			*doubles = takeTurn(properties,players,amntProperties,urandom,corners,current);
			break;
		case 7 :
			status(properties,players,amntProperties,amntPlayers,current);
			*doubles = 1;
			break;
	}
}

void mortgage(struct property **properties,
			  struct player **player,
			  int amntProperties,
			  int current)
{
	if((player[current])->property <= 0)
	{
		puts("You don't own any properties.");
		return;
	}
	houseStatus(0,amntProperties,player,properties,current,&mortgagePrice);
	int select = 0;
	while(1)
	{
		//Select property
		select = 0;
		int range[2] = {1,amntProperties + SIDES};
		parseInput(1,NULL,&select,NULL,range,NULL,NULL);
		if((properties[select-1])->owner == current)
		{
			break;
		}
		else
		{
			puts("You don't own that property.");
		}
	}
	select--;
	if((properties[select])->mortgaged == 0)
	{
		(properties[select])->mortgaged = 1;
		(player[current])->money -= (properties[select])->value * 0.1;
	}
	else if((properties[select])->mortgaged == 1)
	{
		(properties[select])->mortgaged = 0;
		(player[current])->money += (properties[select])->value * 0.1;
	}
	
}

void status(struct property **properties,
			struct player **player,
			int amntProperties,
			int amntPlayers,
			int current)
{
	printf("Your money: $%d\n",(player[current])->money);
	int i = 0;
	while(i < amntPlayers)
	{
		if(i == current)
		{
			i++;
			continue;
		}
		printf("%s\'s money: $%d\n",(player[i])->id,(player[i])->money);
		i++;
	}
	houseStatus(amntPlayers,amntProperties,player,properties,-1,NULL);
}

void mortgagePrice(int *price)
{
	*price *= 0.1;
}

void houseStatus(int amntPlayers,
				 int amntProperties,
				 struct player **player,
				 struct property **properties,
				 int current,
				 void (*changePrice)(int *))
{
	int i = 0;
	for(i = 0;i < amntProperties + SIDES;i++)
	{
		int price = 0;
		if((properties[i])->owner != current && current != -1)
		{
			continue;
		}
		printf("#%d %s",i+1,(properties[i])->name);
		if((properties[i])->owner == -1)
		{
			printf(" Owner: None");
		}
		else
		{
			printf(" Owner: %s",(player[(properties[i])->owner])->id);
		}
		if((properties[i])->mortgaged)
		{
			printf(" Mortgaged: Yes");
		}
		else
		{
			printf(" Mortgaged: No");
		}
		printf(" Hotels: %d",(properties[i])->hotels);
		if((properties[i])->type != 1)
		{
			if(changePrice)
			{
				price = (properties[i])->value;
				(*changePrice)(&price);
				printf(" Price: $%d",price);
			}
			else
			{
				printf(" Price: $%d",(properties[i])->value);
			}
		}
		if(current == -1)
		{
			int k = 0;
			int m = 0;
			int n = 0;
			for(k = 0;k < amntPlayers;k++)
			{
				if(((player[k])->position) -1  == i && (player[k])->playing == 1)
				{
					if(n == 0)
					{
						printf(" Players: ");
						n = 1;
					}
					printf("%s, ",(player[k])->id);
					m = 1;
				}
			}
			puts("");
		}
		if(current != -1)
		{
			puts("");
		}	
	}		
}
void manageHotels(int current,struct player *player,struct property **properties,int amntProperties)
{
	int i = 0;
	int input = 0;
	for(i = 0;i < amntProperties;i++)
	{
		if((properties[i])->owner != current)
		{
			continue;
		}
		int price = (float)(properties[i])->value * ((float)((properties[i])->hotels + 1) * 0.1);
		printf("#%d \"%s\" $%d\n",i+1,(properties[i])->name,price); 
	}
	if(!player->property)
	{
		puts("No properties to manage.");
		return;
	}
	puts("\nPlease enter the ID number for the property you want to modify.");
	int range[2] = {1,amntProperties + SIDES};
	while(1)
	{
		parseInput(1,NULL,&input,NULL,range,NULL,NULL);
		input--;
		if((properties[input])->owner != current)
		{
			puts("You don't own that property.");
			continue;
		}
		else
		{
			break;
		}
	}
	puts("Would you like to Sell or Buy hotels?");
	int retr = getchar();
	int opt;
	while(1)
	{
		while(1)
		{
			int tmp = getchar();
			if(tmp == '\n')
			{
				break;
			}
		}
		if(retr == '\n')
		{
			puts("Unusable input, please enter \'Buy\' or \'Sell\'");
			retr = getchar();
			continue;
		}
		if(retr == 'B' || retr == 'b')
		{
			opt = 1;
			break;
		}
		else if(retr == 's' || retr == 'S')
		{
			opt = -1;
			break;
		}
		else
		{
			puts("Unusable input, please enter \'Buy\' or \'Sell\'");
		}
	}
	puts("How many?");
	int amnt = 0;
	while(1)
	{
		char retr = getchar();
		amnt = 0;
		while(1)
		{
			if(retr == '\n')
			{
				amnt /= 10;
				break;
			}
			amnt += (retr - '0');
			amnt *= 10;
			retr = getchar();
		}
		if(amnt > 0)
		{
			break;
		}
		else
		{
			puts("Unusable input, please enter a number.");
		}
	}
	int price = (float)(properties[input])->value * ((float)((properties[input])->hotels + 1) * 0.1);
	if(opt == 1)
	{
		if(price * amnt > player->money)
		{
			puts("You don't have enough money.");
			return;
		}
		else
		{
			player->money -= (price * amnt);
			(properties[input])->hotels += amnt;
			printf("You now have %d hotel(s)\n",(properties[input])->hotels);
			return;
		}
	}
	if(opt == -1)
	{
		int tmp = (float)(properties[input])->value * ((float)((properties[input])->hotels) * 0.1)
		- (float)(properties[input])->value * ((float)((properties[input])->hotels - 1) * 0.1);
		player->money += (tmp * amnt);
		(properties[input])->hotels -= amnt;
		printf("You now have %d hotel(s)\n",(properties[input])->hotels);
	}	
	
				
}
	
			
int takeTurn(struct property **properties,struct player **player,int amntProperties,FILE *urandom,int *corners,int current)
{
	int diceOne = rollDice(urandom,1,6);
	int diceTwo = rollDice(urandom,1,6);
	int whatReturn = 0;
	int corn = 0;
	char retr = 0;
	char res = 0;
	int purchase = 0;
	
	printf("\"%s\", you rolled a %d and a %d\n",(player[current])->id,diceOne,diceTwo);
	if(diceOne == diceTwo)
	{
		whatReturn = 1;
	}
	(player[current])->position += diceOne + diceTwo;
	if((player[current])->position > amntProperties + SIDES)
	{
		(player[current])->position -= amntProperties + SIDES;
	}
	if((properties[(player[current])->position-1])->type == 1)
	{
		corn = 1;
	}
	if(corn == 0 && (properties[(player[current])->position-1])->owner == -1)
	{
		while(1)
		{
			printf("\"%s\", would you like to buy \"%s\" on position #%d for $%d\n",
			(player[current])->id,(properties[(player[current])->position-1])->name,
			(player[current])->position,(properties[(player[current])->position-1])->value);
			retr = getchar();
			if(retr == '\n')
			{
				puts("Please enter Y for yes or N for no");
			}
			else
			{
				while(1)
				{
					res = getchar();
					if(res == '\n')
					{
						break;
					}
				}
			}
			if(retr == 'Y' || retr == 'y')
			{
				purchase = 1;
				break;
			}
			else if(retr == 'N' || retr == 'n')
			{
				purchase = 0;
				break;
			}
			else
			{
				puts("Please enter Y for yes or N for no");
			}
		}
	}
	else if(corn == 0)
	{
		printf("\"%s\", you are currently on position #%d which is owned by \"%s\",\nit currently "
		,(player[current])->id,(player[current])->position,(player[(properties[(player[current])->position-1])->owner])->id);
	    if((properties[(player[current])->position-1])->mortgaged)
	    {
	    	puts("is mortgaged.");
    	}
    	else
    	{
    		puts("isn't mortgaged.");
		}
		if((properties[(player[current])->position-1])->mortgaged == 0)
		{
			int tmp = (properties[(player[current])->position-1])->value / 5;
			printf("For landing on rented property, you are being charge $%d\n",tmp);
			(player[current])->money -= tmp;
		}
	}
	else
	{
		puts("You are currently on a corner.");
	}
	if(purchase)
	{
		int result = (player[current])->money - (properties[(player[current])->position-1])->value;
		if(result >= 0)
		{
			(player[current])->money -= (properties[(player[current])->position-1])->value;
			(properties[(player[current])->position-1])->owner = current;
			printf("Congratulations \"%s\", you now own the property!\n",(player[current])->id);
		}
		else
		{
			printf("\"%s\", you currently only have $%d in funds,\nyou cannot purchase this property\n",
			(player[current])->id,(player[current])->money);
			whatReturn = 1;
		}
		
	}
	
	return whatReturn;
}

void playerResign(int current,struct player *players,struct property **property,int amount)
{
	int i = 0;
	
	players->playing = 0;
	for(i = 0;i < amount + SIDES;i++)
	{
		if((property[i])->owner == current)
		{
			(property[i])->owner = -1;
		}
	}
}

char queryPlayer(char *name)
{
	char retr = 0;
	char res = 0;
	
	printf("\nPlayer \"%s\", please pick an option:\n",name);
	puts("1: resign, 2: manage mortgages, 3: manage hotels");
	puts("4: trade, 5: auction properties, 6: roll dice");
	puts("7: status");
	while(1)
	{
		retr = getchar();
		while(1)
		{
			if(res == '\n' || retr == '\n')
			{
				res = 0;
				break;
			}
			res = getchar();
		}
		if(retr < '1' || retr > '7')
		{
			puts("Please enter a number from 1 through to 7");
		}
		else
		{
			break;
		}
	}
	return retr - '0';
}

void setFourCorners(int properties,int *corners)
{
	int i = 0;
	int *tmp = malloc(4 * sizeof(int));
	
	properties += 4;
	for(;properties > 0;properties--)
	{
		if(i > SIDES-1)
		{
			i = 0;
		}
		(corners[i])++;
		i++;
	}
	for(i = 0;i < 4;i++)
	{
		tmp[i] = corners[i];
	}
	for(i = 0;i < SIDES-1;i++)
	{
		if(i == 0)
		{
			corners[i] = 1;
		}
		corners[i+1] = corners[i] + tmp[i];
	}
		
}
	

int setupPlayers(struct player ***players,int *amntPlayers)
{
	*amntPlayers = 0;
	int forAll = 0;
	unsigned int oldMoney = 0;
	while(1)
	{
		char retr = 0;
		int i = 0;
		int leave = 0;
		unsigned int money = 0;
		char *name = malloc(13);
		
		if(*amntPlayers != 0)
		{
			puts("Add a player? Yes or No");
			while(1)
			{
				retr = getchar();
				while(1)
				{
					char res = getchar();
					if(res == '\n')
					{
						break;
					}
				}
				if(retr == 'Y' || retr == 'y')
				{
					leave = 0;
					break;
				}
				else if(retr == 'N' || retr == 'n')
				{
					leave = 1;
					break;
				}
				else
				{	
					puts("ERROR: unable to decipher input :: enter Y for Yes or N for No");
					printf("Yes or No? ");
				}
			}
		}
		if(leave == 1)
		{
			free(name);
			break;
		}
		printf("Please enter player #%d's name, max 12 characters\n",(*amntPlayers)+1);
		while(1)
		{
			retr = getchar();
			if(retr == '\n' || i >= 12)
			{
				break;
			}
			name[i] = retr;
			i++;
		}
		name[i] = '\0';
		if(allocPlayerArray(players,*amntPlayers))
		{
			puts("ERROR: error from allocPlayerArray() :: returned non-zero");
			return 1;
		}
		if(i >= 12)
		{
			while(1)
			{
				retr = getchar();
				if(retr == '\n')
				{
					break;
				}
			}
		}
		if(forAll == 0)
		{
			puts("Do you want money given, to be equal?");
			while(1)
			{
				retr = getchar();
				while(1)
				{
					char res = getchar();
					if(res == '\n')
					{
						break;
					}
				}
				if(retr == 'Y' || retr == 'y')
				{
					forAll = 1;
					break;
				}
				else if(retr == 'N' || retr == 'n')
				{
					forAll = -1;
					break;
				}
				else
				{	
					puts("ERROR: unable to decipher input :: enter Y for Yes or N for No");
				}
			}
		}
		if(forAll == -1 || forAll == 1)
		{
			if(forAll == 1)
			{
				forAll++;
			}
			puts("Please enter the amount of money for players");
			while(1)
			{
				retr = getchar();
				if(retr == '\n')
				{
					break;
				}
				money += retr - '0';
				retr = getchar();
				if(retr != '\n')
				{
					money *= 10;
					ungetc(retr,stdin);
				}
				else
				{
					break;
				}
			}
		}
		if(forAll == 2)
		{
			oldMoney = money;
			forAll++;
		}
		if(forAll >= 2)
		{
			money = oldMoney;
		}
		if(setupPlayerStruct(*players,money,name,*amntPlayers))
		{
			puts("ERROR: error from setupPlayerStruct :: non-zero returned");
			return 1;
		}
		(*players)[*amntPlayers]->position = 1;
		(*players)[*amntPlayers]->playing = 1;
		
		(*amntPlayers)++;	
		free(name);	
		name = NULL;
	}
	return 0;
}

int setupPlayerStruct(struct player **array,unsigned int money,char *name,int players)
{
	int i = 0;

	(array[players])->money = money;
	(array[players])->id = malloc(13);
	while(1)
	{
		if(name[i] == '\0')
		{
			(array[players])->id[i] = name[i];
			break;
		}
		(array[players])->id[i] = name[i];
		i++;
	}
	return 0;
}	
		
int allocPlayerArray(struct player ***players,int amntPlayers)
{
	void *tmp;
	int i = 0;
	amntPlayers++;
	
	tmp = realloc(*players,sizeof(void *) * amntPlayers);
	if(tmp == NULL)
	{
		puts("ERROR: error in allocPlayerArray() :: realloc returned NULL");
		return 1;
	}
	*players = tmp;
	(*players)[amntPlayers-1] = 0;
	for(i = 0;i < amntPlayers;i++)
	{
		tmp = realloc((*players)[i],sizeof(struct player));
		if(tmp == NULL)
		{
			puts("ERROR: error in allocPlayerArray() :: realloc returned NULL");
			return 1;
		}
		(*players)[i] = tmp;
	}
	return 0;
}
int setupTmpPropertyNames(struct property **array,int amntOfProperties)
{
	int i = 0;
	int j = 1;
		
	while(i < amntOfProperties)
	{
		if(*(array[i]->name) == '\0')
		{
			if(strcpy(array[i]->name,"Temporary House #") == NULL)
			{
				return 1;
			}		 	
			array[i]->name[17] = j + '0';
			j++;
		 }
		 i++;
	 }
	 return 0;
}

int needMore(void **array,int *used)
{
	void *tmp;
	tmp = realloc(*array,*used * 2 * sizeof(int *));
	if(tmp != NULL)
	{
		*used *= 2;
		*array = tmp;
	}
	else
	{
		return 1;
	}
	return 0;
}
int FillArryPropertyData(struct property **array,int amntOfProperties,FILE *fp)
{
	int bufferUsed = 100;
	int freeUsed = 100;
	int freeNameUsed = 100;
	int i = 0;
	int j = 0;
	int k = 0;
	int returnValue = 0;
	int returnValue2 = 0;
	unsigned int money = 0;
	char *name;
	char *buffer;
	void **freePtrs;
	void **freeNamePtrs;
	rewind(fp);
	
	freePtrs = malloc(freeUsed * sizeof(int *));
	freeNamePtrs = malloc(freeNameUsed * sizeof(int *));
	while(returnValue == 0 || returnValue == -1)
	{
		buffer = malloc(bufferUsed);
		if(i >= freeUsed)
		{
			if(needMore((void **)&freePtrs,&freeUsed))
			{
				puts("ERROR: error from needMore() :: non-zero returned");
				return -5;
			}
		}
		
		freePtrs[j] = buffer;
		j++;
		returnValue = bufferPropertyFile(fp,&buffer,bufferUsed);
		if(returnValue && returnValue != -1 && returnValue != -3)
		{
			puts("ERROR: error from bufferPropertyFile() :: non-zero returned");
			return -1;
		}
		if(returnValue == -3)
		{
			break;
		}		
		if(returnValue == -1)
		{
			continue;
		}
		if(buffer[0] == '#')
		{
			continue;
			
		}
		returnValue2 = splitBuffer(buffer,&money,&name);
		if(k >= freeUsed)
		{
			if(needMore((void **)&freeNamePtrs,&freeNameUsed))
			{
				puts("ERROR: error from needMore() :: non-zero returned");
				return -5;
			}
		}
		freeNamePtrs[k] = name;
		k++;
		if(returnValue2 == -3)
		{
			return 1;
		}
		else if(returnValue2)
		{
			puts("ERROR: error from splitBuffer() :: non-zero-returned");
			return 1;
		}
		if(fillPropertyStruct(array[i],money,name))
		{
			puts("ERROR: error from fillPropertyStruct()  :: non-zero returned");
			return 1;
		}
		money = 0;
		i++;
	}
	for(i = 0;i < j;i++)
	{		
		free(freePtrs[i]);
		freePtrs[i] = NULL;
	}
	free(freePtrs);
	freePtrs = NULL;
	for(i = 0;i < k;i++)
	{
		free(freeNamePtrs[i]);
		freeNamePtrs[i] = NULL;
	}
	free(freeNamePtrs);
	freeNamePtrs = NULL;
	
	return 0;
}

int fillPropertyStruct(struct property *array,unsigned int money,char *name)
{
	int nameSize = 100;
	int i = 0;
	array->name = malloc(nameSize);
	array->value = money;
	array->type = 0;
	array->owner = -1;
	array->mortgaged = 0;
	array->hotels = 0;
	while(1)
	{
		if(i >= nameSize)
		{
			void *tmp = realloc(array->name,nameSize * 2);
			nameSize *= 2;
			if(tmp)
			{
				array->name = tmp;
			}
			else
			{
				return -1;
			}
		}
		if(name[i] == '\0')
		{
			break;
		}
		array->name[i] = name[i];
		i++;
	}
	array->name[i] = '\0';
	
	return 0;
}
int splitBuffer(char *buffer,unsigned int *money,char **name)
{
	int i = 0;
	int j = 1;
	int nameSize = 100;
	
	*name = malloc(nameSize);
	while(1)
	{
		if(buffer[j] != '"')
		{
			(*name)[j-1] = buffer[j];
		}
		else
		{
			i++;
		}
		j++;
		if(i)
		{
			break;
		}
		if(j >= nameSize)
		{
			void *tmp = 0;
			tmp = realloc(*name,nameSize * 2);
			nameSize = nameSize * 2;
			if(tmp != NULL)
			{
				*name = tmp;
			}
			else
			{
				puts("ERROR: error in splitBuffer :: realloc returned NULL");
				return -1;
			}
		}
	}
	(*name)[j-1] = '\0';
	
	while(buffer[j] != '$')
	{
		if(buffer[j] == '\0')
		{
			puts("ERROR: error in splitBuffer() :: no $ found in current property file line");
			return -2;
		}
		j++;
	}
	j++;
	while(buffer[j] != ' ' && buffer[j] != '\0')
	{
		
		/*//DEBUG
		if(buffer[j] == '\0')
		{
			printf("\'NULL\'");
		}
		else
		{
			printf("%c",buffer[j]);
		}
		//END DEBUG*/
		
		if(buffer[j] < 48 || buffer[j] > 57)
		{
			puts("ERROR: error in splitBuffer() :: property file in wrong format");
			return -3;
		}
		*money += (buffer[j] - '0');
		if(buffer[j+1] >= 48 && buffer[j+1] <= 57)
		{
			*money *= 10;
		}
		j++;
	}
	//DEBUG
	/*printf("BUFFER: %s\n",buffer);*/
	
	return 0;
}

int bufferPropertyFile(FILE *fp,char **buffer,int i)
{	
	int j = (i - i);
	
	if(feof(fp))
	{
		return -3;
	}
	char retr = 0;
	while(1)
	{
		if(j + 1 >= i)
		{
			void *tmp = realloc(*buffer,i * 2);
			if(tmp != NULL)
			{
				*buffer = tmp;
				i = i * 2;
			}
			else
			{
				puts("ERROR: error in bufferPropertyFile() :: realloc returned NULL");
				return -2;
			}
		}
		retr = fgetc(fp);
		if(retr == '\n' || feof(fp))
		{
			break;
		}
		(*buffer)[j] = retr;
		j++;
	}
	(*buffer)[j] = '\0';
	if(**buffer == '\0')
	{
		return -1;
	}
	return 0;
}
		
int rollDice(FILE *fp, int low,int high)
{
	int seed = fgetc(fp);
	srand(seed);
	return (rand() % (high - low)) + low + 1;
}

int amountOfLines(FILE *file,int *total)
{
	int i = 0;
	char retr = 0;
	int currentLineComment = 0;
	char prevRetr = 0;
	
	while(1)
	{
		retr = fgetc(file);
		if(retr != '#')
		{
			if(retr == EOF)
			{
				break;
			}
			if(retr == '\n' )
			{
				if(currentLineComment == 0 && prevRetr != '\n')
				{
					i++;
				}
				else
				{
					currentLineComment = 0;
				}
				(*total)++;
			}
		}
		else
		{
			currentLineComment = 1;
		}
		prevRetr = retr;
	}
	return i;
}
int createArrayOfPtrs(int numberOfPointers,void ***pointer)
{
	void *tmp = malloc(numberOfPointers * sizeof (tmp));
	if(tmp != NULL)
	{
		*pointer = tmp;
		/*//DEBUG
		printf("Pointer: %p\n",*pointer);*/
	}
	else
	{
		return 1;
	}
	return 0;
}

int makeArryOfPropertyPtrs(int numberOfPointers,struct property **pointer)
{
	int i = 0;
	void *tmp;
	for(i = 0;i < numberOfPointers;i++)
	{
		tmp = malloc(sizeof(struct property));
		if(tmp == NULL)
		{
			return 1;
		}
		pointer[i] = (struct property *)tmp;
	}
	return 0;
}
