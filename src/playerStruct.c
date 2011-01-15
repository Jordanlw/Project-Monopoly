
int setupPlayers(struct player ***players,int *amntPlayers)
{
	*amntPlayers = 0;
	int forAll = 0;
	unsigned int oldMoney = 0;
	while(1)
	{
		char retr = 0;
		int i = 0;
		int leave = 0;
		unsigned int money = 0;
		char *name = malloc(13);
		
		if(*amntPlayers != 0)
		{
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
		if(forAll == 0)
		{
			puts("Do you want money given, to be equal?");
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
					forAll = 1;
					break;
				}
				else if(retr == 'N' || retr == 'n')
				{
					forAll = -1;
					break;
				}
				else
				{	
					puts("ERROR: unable to decipher input :: enter Y for Yes or N for No");
				}
			}
		}
		if(forAll == -1 || forAll == 1)
		{
			if(forAll == 1)
			{
				forAll++;
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
		}
		if(forAll == 2)
		{
			oldMoney = money;
			forAll++;
		}
		if(forAll >= 2)
		{
			money = oldMoney;
		}
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

