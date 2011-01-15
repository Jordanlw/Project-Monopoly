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

