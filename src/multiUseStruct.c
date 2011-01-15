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

