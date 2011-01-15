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

