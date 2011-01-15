
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

