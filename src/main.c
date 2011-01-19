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

