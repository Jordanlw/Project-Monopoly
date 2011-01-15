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

