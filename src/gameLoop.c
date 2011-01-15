
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
	int previous = -1;
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
		bankruptcy(properties,players,amntProperties,amntPlayers,currentPlayer,doubles,&previous);
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
	
int rollDice(FILE *fp, int low,int high)
{
	int seed = fgetc(fp);
	srand(seed);
	return (rand() % (high - low)) + low + 1;
}
