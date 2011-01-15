#include "mainHeader.h"

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

void bankruptcy(struct property **properties,
				struct player **players,
				int amntProperties,
				int amntPlayers,
				int currentPlayer,
				int *doubles,
				int *previous)
{
	if((players[currentPlayer])->money >= 0)
	{
		return;
	}
	if(*previous == currentPlayer)
	{
		playerResign(currentPlayer,players[currentPlayer],properties,amntProperties);
		puts("\nYou have resigned for not clearing your debt in time.");
		*doubles = 0;
		return;
	}
	puts("\nYou currently have negative money,\nplease in the current turn achieve a positive money value,\n"
	"if you don't you will automatically resign from the game.");
	*doubles = 1;
	*previous = currentPlayer;
	puts("Press any button to continue.");
	while(1)
	{
		char retr = getchar();
		if(retr == '\n')
		{
			break;
		}
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

