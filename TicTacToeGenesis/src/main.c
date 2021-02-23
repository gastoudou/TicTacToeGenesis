#include "gameState.h"

int main()
{
	TTT_GameState gameState;

	Init(&gameState);
	while (1)
	{
		Update(&gameState);
	}

	DeInit(&gameState);

	return 0;
}
