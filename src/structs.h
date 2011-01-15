struct property
{
	char *name;
	unsigned int value;
	int owner;
	unsigned int type;
	int mortgaged;
	int hotels;
};

struct player
{
	char *id;
	int money;
	unsigned int position;
	int playing;
	int property;
};
