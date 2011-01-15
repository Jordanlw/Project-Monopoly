
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

void mortgagePrice(int *price)
{
	*price *= 0.1;
}

