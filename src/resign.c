	
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

