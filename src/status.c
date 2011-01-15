
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

