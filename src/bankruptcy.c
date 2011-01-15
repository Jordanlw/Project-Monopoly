
void bankruptcy(struct property **properties,
				struct player **players,
				int amntProperties,
				int amntPlayers,
				int currentPlayer,
				int *doubles,
				int *previous)
{
	if((players[currentPlayer])->money >= 0)
	{
		return;
	}
	if(*previous == currentPlayer)
	{
		playerResign(currentPlayer,players[currentPlayer],properties,amntProperties);
		puts("\nYou have resigned for not clearing your debt in time.");
		*doubles = 0;
		return;
	}
	puts("\nYou currently have negative money,\nplease in the current turn achieve a positive money value,\n"
	"if you don't you will automatically resign from the game.");
	*doubles = 1;
	*previous = currentPlayer;
	puts("Press any button to continue.");
	while(1)
	{
		char retr = getchar();
		if(retr == '\n')
		{
			break;
		}
	}
}

