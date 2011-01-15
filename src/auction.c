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
	
