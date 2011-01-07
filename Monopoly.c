#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
void houseStatus(int,int ,struct player **,struct property **,int);
void mortgage(struct property **,struct player **,int ,int);
void addCorners(int ,int *,struct property ***);

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
	for(i = 0;i < linesInPropertyFile;i++)
	{
		free((arrayForProperties[i])->name);
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
		i = 0;
		int action = queryPlayer((players[currentPlayer])->id);
		actOnAction(properties,players,amntProperties,amntPlayers,corners,action,currentPlayer,urandom,doubles);
	}
	return 0;
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
			break;
		case 3 :
			manageHotels(current,players[current],properties,amntProperties);
			break;
		case 6 :
			*doubles = takeTurn(properties,players,amntProperties,urandom,corners,current);
			break;
		case 7 :
			status(properties,players,amntProperties,amntPlayers,current);
			break;
	}
}

void mortgage(struct property **properties,
			  struct player **player,
			  int amntProperties,
			  int current)
{
	int i = 0;
	int present = 0;
	char retr = 0;
	while(1)
	{
		while(1)
		{
			if(i >= amntProperties)
			{
				break;
			}
			if((properties[i])->owner == current)
			{
				present = 1;
				break;
			}
			i++;
		}
		if(present == 0)
		{
			puts("You don't own any houses, Can not mortage yet.");
			return;
		}	
		houseStatus(0,amntProperties,player,properties,current);
		i = 0;
		puts("Please select a property.");
		while(1)
		{
			retr = getchar();
			if(retr == '\n')
			{
				i /= 10;
				break;
			}
			i += retr - '0';
			i *= 10;
		}
		if(i < 1 || i > amntProperties)
		{
			puts("Please enter a number corresponding to the property.");
		}
		break;
	}
	i--;
	if((properties[i])->hotels > 0)
	{
		puts("You need to sell all hotels on this property before mortgaging.");
	}
	else
	{
		if((properties[i])->owner != current)
		{
			puts("You don't own that property.");
			return;
		}
		if((properties[i])->mortgaged == 1)
		{
			(player[current])->money += (properties[i])->value / 10;
			(properties[i])->mortgaged = 0;
			puts("You have unmortgaged this property.");
		}
		else
		{
			(player[current])->money -= (properties[i])->value / 10;
			(properties[i])->mortgaged = 1;
			puts("You have mortgaged this property.");
		}
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
	houseStatus(amntPlayers,amntProperties,player,properties,-1);
}

void houseStatus(int amntPlayers,
				 int amntProperties,
				 struct player **player,
				 struct property **properties,
				 int current)
{
	int i = 0;
	for(i = 0;i < amntProperties + SIDES;i++)
	{
		if((properties[i])->owner != current && current != -1)
		{
			i++;
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
		if(current == -1)
		{
			int k = 0;
			int m = 0;
			int n = 0;
			for(k = 0;k < amntPlayers;k++)
			{
				if(((player[k])->position) -1  == i)
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
			if(m == 0)
			{
				puts("");
			}
			else
			{
				puts("");
			}
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
	int present = 0;
	int input = 0;
	while(1)
	{
		if(i >= amntProperties)
		{
			break;
		}
		if((properties[i])->owner != current)
		{
			i++;
			continue;
		}
		present = 1;
		int price = (float)(properties[i])->value * ((float)((properties[i])->hotels + 1) * 0.1);
		printf("#%d \"%s\" $%d Hotels:%d\n",i+1,(properties[i])->name,price,(properties[i])->hotels); 
		i++;
	}
	if(present == 0)
	{
		puts("No properties to manage.");
		return;
	}
	puts("\nPlease enter the ID number for the property you want to modify.");
	while(1)
	{
		char retr = getchar();
		int error = 0;
		input = 0;
		while(1)
		{
			if(retr == '\n')
			{
				input /= 10;
				input--;
				break;
			}
			input += (retr - '0');
			input *= 10;
			retr = getchar();
		}
		if(input > 0 && input <= i)
		{
			if((properties[input])->owner == current)
			{
				break;
			}
			error = 1;
		}
		else
		{
			puts("Unusable input, please enter the ID number of a property.");
		}
		if(error)
		{
			error = 0;
			puts("Unusable input, please enter the ID number of a property.");
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
	int price = (float)(properties[input])->value * ((float)((properties[input])->hotels + 1) * 0.1);
	if(opt == 1)
	{
		if(price > player->money)
		{
			puts("You don't have enough money.");
			return;
		}
		else
		{
			player->money -= price;
			(properties[input])->hotels++;
			printf("You now have %d hotel(s)\n",(properties[input])->hotels);
			return;
		}
	}
	if(opt == -1)
	{
		int tmp = (float)(properties[input])->value * ((float)((properties[input])->hotels) * 0.1)
		- (float)(properties[input])->value * ((float)((properties[input])->hotels - 1) * 0.1);
		player->money += tmp;
		(properties[input])->hotels--;
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
		}
		
	}
	
	return whatReturn;
}

void playerResign(int current,struct player *players,struct property **property,int amount)
{
	int i = 0;
	
	players->playing = 0;
	for(i = 0;i < amount;i++)
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
	while(1)
	{
		char retr = 0;
		int i = 0;
		int leave = 0;
		unsigned int money = 0;
		char *name = malloc(13);
		
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
		//DEBUG
		//printf("%d\n",money);
		
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
