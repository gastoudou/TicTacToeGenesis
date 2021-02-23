#include "gameState.h"

#include <genesis.h>
#include "sprite.h"
#include "gfx.h"

s16 moveUp = 0;
s16 moveDown = 0;
s16 moveRight = 0;
s16 moveLeft = 0;
s16 gameplayStarted = 0;
s16 pressedA = 0;
s16 canPressA = 1;
s16 turn = 0;
s16 showDebug = 0;
s16 gameOver = 0;
s16 gridChecks[9] = { 0 };
s16 level = 1;
s16 maxLevel = 2;

s16 canGoUp = 1;
s16 canGoDown = 1;

s16 rounds[9] = { 0 };
s16 currentRound = 0;

s16 gridCoords[9][2] = {
{ 108, 48 },
{ 146, 48 },
{ 181, 48 },
{ 108, 87 },
{ 146, 87 },
{ 181, 87 },
{ 108, 125 },
{ 146, 125 },
{ 181, 125 }
};

Sprite* gridSprites[9] = { NULL };
u8 currentSprite = 0;

#define MENU 0
#define GAMEPLAY 1

s16 Evaluate(s16 board[])
{
	if ((gridChecks[0] == gridChecks[1] && gridChecks[0] == gridChecks[2] && gridChecks[0] == 1)
		|| (gridChecks[3] == gridChecks[4] && gridChecks[3] == gridChecks[5] && gridChecks[3] == 1)
		|| (gridChecks[6] == gridChecks[7] && gridChecks[6] == gridChecks[8] && gridChecks[6] == 1)
		|| (gridChecks[0] == gridChecks[3] && gridChecks[0] == gridChecks[6] && gridChecks[0] == 1)
		|| (gridChecks[1] == gridChecks[4] && gridChecks[1] == gridChecks[7] && gridChecks[1] == 1)
		|| (gridChecks[2] == gridChecks[5] && gridChecks[2] == gridChecks[8] && gridChecks[2] == 1)
		|| (gridChecks[0] == gridChecks[4] && gridChecks[0] == gridChecks[8] && gridChecks[0] == 1)
		|| (gridChecks[2] == gridChecks[4] && gridChecks[2] == gridChecks[6] && gridChecks[2] == 1))
	{
		return 10;
	}

	if ((gridChecks[0] == gridChecks[1] && gridChecks[0] == gridChecks[2] && gridChecks[0] == -1)
		|| (gridChecks[3] == gridChecks[4] && gridChecks[3] == gridChecks[5] && gridChecks[3] == -1)
		|| (gridChecks[6] == gridChecks[7] && gridChecks[6] == gridChecks[8] && gridChecks[6] == -1)
		|| (gridChecks[0] == gridChecks[3] && gridChecks[0] == gridChecks[6] && gridChecks[0] == -1)
		|| (gridChecks[1] == gridChecks[4] && gridChecks[1] == gridChecks[7] && gridChecks[1] == -1)
		|| (gridChecks[2] == gridChecks[5] && gridChecks[2] == gridChecks[8] && gridChecks[2] == -1)
		|| (gridChecks[0] == gridChecks[4] && gridChecks[0] == gridChecks[8] && gridChecks[0] == -1)
		|| (gridChecks[2] == gridChecks[4] && gridChecks[2] == gridChecks[6] && gridChecks[2] == -1))
	{
		return -10;
	}

	return 0;
}

s16 IsMovesLeft(s16 board[])
{
	for (s16 i = 0; i < 9; ++i)
	{
		if (board[i] == 0)
			return 1;
	}
	return 0;
}

s16 MinMax(s16 board[], s16 depth, s16 isMax, s16 alpha, s16 beta )
{
	if (level == 1)
	{
		if (depth > 2)
		{
			return 0;
		}
	}

	s16 score = Evaluate(board);

	if (score == 10) return score;
	if (score == -10) return score;
	if (IsMovesLeft(board)==0) return 0;

	if (isMax == 1)
	{
		s16 best = -124;

		for (s16 i = 0; i < 9; ++i)
		{
			if (board[i] == 0)
			{
				board[i] = 1;
				best = max(best, MinMax( board, depth + 1, 0, alpha, beta ) );
				board[i] = 0;
				alpha = max(alpha, best);
				if (beta <= alpha)
					break;
			}
		}
		return best;
	}
	else
	{
		s16 best = 124;

		for (s16 i = 0; i < 9; ++i)
		{
			if (board[i] == 0)
			{
				board[i] = -1;
				best = min(best, MinMax( board, depth + 1, 1, alpha, beta) );
				board[i] = 0;
				beta = min(beta, best);
				if (beta <= alpha)
					break;
			}
		}
		return best;
	}
}

void AITurn_Level0()
{
	s16 choice = random() % 9;
	while (gridChecks[choice] != 0)
	{
		choice = random() % 9;
	}

	gridChecks[choice] = -1;
	rounds[currentRound++] = choice;
	gridSprites[currentSprite++] = SPR_addSprite(&round, gridCoords[choice][0], gridCoords[choice][1], TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
	turn = 0;
}

void AITurn_Level1()
{
	s16 choice = 10;
	s16 best = 124;

	for (int i = 0; i < 9; ++i)
	{
		if (gridChecks[i] == 0)
		{
			gridChecks[i] = -1;
			s16 potential = MinMax(gridChecks, 0, 1, -124, +124);
			gridChecks[i] = 0;

			if (potential < best)
			{
				choice = i;
				best = potential;
			}
		}
	}
	if (choice < 10)
	{
		gridChecks[choice] = -1;
		rounds[currentRound++] = choice;
		gridSprites[currentSprite++] = SPR_addSprite(&round, gridCoords[choice][0], gridCoords[choice][1], TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
		turn = 0;
	}
	else
	{
		AITurn_Level0();
	}
}

void Reset()
{
	ClearText();
	u16 ind = 0u;
	VDP_drawImageEx(PLAN_A, &grid, TILE_ATTR_FULL(PAL2, FALSE, FALSE, FALSE, ind), 12, 5, TRUE, CPU);

	gameplayStarted = 0;
	turn = 0;
	gameOver = 0;
	memset(gridChecks, 0, sizeof(s16) * 9);
	memset(rounds, 0, sizeof(s16) * 9);
	currentRound = 0;
	for (int i = 0; i < 9; ++i)
	{
		if (gridSprites[i])
		{
			SPR_releaseSprite(gridSprites[i]);
			gridSprites[i] = NULL;
		}
	}
	currentSprite = 0;
}

void AITurn()
{
	if (level == 0)
		AITurn_Level0();
	else
		AITurn_Level1();
}

void TryClic(u32 x, u32 y)
{
	if (turn == 1)
		return;

	s16 addSprite = 0;
	u32 spriteX = 0;
	u32 spriteY = 0;
	s16 cell = 250;

	// a | b | c
	// ---------
	// d | e | f
	// ---------
	// g | h | i

	//a: 108-48 > 137-80
	if (x > 108 && x < 137 && y > 48 && y < 80)
	{
		if (gridChecks[0] == 0)
		{
			addSprite = 1;
			spriteX = 108;
			spriteY = 48;
			gridChecks[0] = 1;
			cell = 0;
		}
	}

	//b: 146-48 > 177-80
	if (x > 146 && x < 177 && y > 48 && y < 80)
	{
		if (gridChecks[1] == 0)
		{
			addSprite = 1;
			spriteX = 146;
			spriteY = 48;
			gridChecks[1] = 1;
			cell = 1;
		}

	}

	//c: 181-48 > 216-80
	if (x > 181 && x < 216 && y > 48 && y < 80)
	{
		if (gridChecks[2] == 0)
		{
			addSprite = 1;
			spriteX = 181;
			spriteY = 48;
			gridChecks[2] = 1;
			cell = 2;
		}
	}

	//d: 108-87 > 137-120
	if (x > 108 && x < 137 && y > 87 && y < 120)
	{
		if (gridChecks[3] == 0)
		{
			addSprite = 1;
			spriteX = 108;
			spriteY = 87;
			gridChecks[3] = 1;
			cell = 3;
		}
	}

	//e: 146-87 > 177-120
	if (x > 146 && x < 177 && y > 87 && y < 120)
	{
		if (gridChecks[4] == 0)
		{
			addSprite = 1;
			spriteX = 146;
			spriteY = 87;
			gridChecks[4] = 1;
			cell = 4;
		}
	}

	//f: 181-87 > 216-120
	if (x > 181 && x < 216 && y > 87 && y < 120)
	{
		if (gridChecks[5] == 0)
		{
			addSprite = 1;
			spriteX = 181;
			spriteY = 87;
			gridChecks[5] = 1;
			cell = 5;
		}
	}

	//g: 108-125 > 137-160
	if (x > 108 && x < 137 && y > 125 && y < 160)
	{
		if (gridChecks[6] == 0)
		{
			addSprite = 1;
			spriteX = 108;
			spriteY = 125;
			gridChecks[6] = 1;
			cell = 6;
		}
	}

	//h: 146-125 > 177-160
	if (x > 146 && x < 177 && y > 125 && y < 160)
	{
		if (gridChecks[7] == 0)
		{
			addSprite = 1;
			spriteX = 146;
			spriteY = 125;
			gridChecks[7] = 1;
			cell = 7;
		}
	}

	//i: 181-125 > 216-160
	if (x > 181 && x < 216 && y > 125 && y < 160)
	{
		if (gridChecks[8] == 0)
		{
			addSprite = 1;
			spriteX = 181;
			spriteY = 125;
			gridChecks[8] = 1;
			cell = 8;
		}
	}

	if (addSprite == 1)
	{
		gridSprites[currentSprite++] = SPR_addSprite(&cross, spriteX, spriteY, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
		rounds[currentRound++] = cell;
		turn = 1 - turn;
	}
}

void myJoyHandler(u16 joy, u16 changed, u16 state)
{
	if (joy == JOY_1)
	{
		if (state & BUTTON_UP)
		{
			moveUp = 1;
		}
		else if (changed & BUTTON_UP)
		{
			moveUp = 0;
		}

		if (state & BUTTON_DOWN)
		{
			moveDown = 1;
		}
		else if (changed & BUTTON_DOWN)
		{
			moveDown = 0;
		}

		if (state & BUTTON_RIGHT)
		{
			moveRight = 1;
		}
		else if (changed & BUTTON_RIGHT)
		{
			moveRight = 0;
		}

		if (state & BUTTON_LEFT)
		{
			moveLeft = 1;
		}
		else if (changed & BUTTON_LEFT)
		{
			moveLeft = 0;
		}

		if (state & BUTTON_START)
		{
			if (gameplayStarted == 0)
			{
				gameplayStarted = 1;
			}
		}
		else if (changed & BUTTON_START)
		{
			gameplayStarted = 0;
		}

		if (state & BUTTON_A && canPressA == 1)
		{
			pressedA = 1;
			canPressA = 0;
		}
		else if (changed & BUTTON_A)
		{
			canPressA = 1;
		}
	}
}

void ClearText()
{
	int i;
	for (i = 0; i < 96; ++i)
	{
		VDP_clearText(0, i, 256);
	}
}

void UpdateGameplay(TTT_GameState* gameState)
{
	u16 ind = 0;

	if (gameState->mySprite == NULL)
	{
		VDP_drawImageEx(PLAN_A, &grid, TILE_ATTR_FULL(PAL2, FALSE, FALSE, FALSE, ind), 0, 0, TRUE, CPU);
		gameState->mySprite = SPR_addSprite(&hand, gameState->posX, gameState->posY, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
	}

	if (gameOver == 0)
	{
		s16 score = Evaluate(gridChecks);

		if (score == 10) gameOver = 1;
		if (score == -10) gameOver = 2;
	}

	s16 isMoveLeft = IsMovesLeft(gridChecks);

	if (isMoveLeft == 0 || gameOver > 0)
	{
		VDP_drawText("GAME OVER", 1, 15);
		if (gameOver >= 1)
		{
			{
				char buffer[64];
				sprintf(buffer, gameOver == 1 ? "Cross wins!" : "Round wins!");
				VDP_drawText(buffer, 1, 17);
			}
		}
		else
		{
			VDP_drawText("It's a tie!", 1, 17);
		}
		VDP_drawText("Press START to restart", 5, 23);
		
		if (gameplayStarted)
		{
			Reset();
		}

		return;
	}

	if (turn == 1) // AI TURN, pick a empty element
	{
		AITurn();
		return;
	}

	int deltaX = moveRight - moveLeft;

	gameState->posY -= moveUp - moveDown;
	gameState->mySprite->x = gameState->posX;
	gameState->mySprite->y = gameState->posY;
	gameState->posX += deltaX;

	if (pressedA == 1 && gameOver == 0)
	{
		TryClic(gameState->posX, gameState->posY);
		pressedA = 0;
	}

	SPR_setPosition(gameState->mySprite, gameState->posX, gameState->posY);
}

void UpdateMenu(TTT_GameState* gameState)
{
	VDP_setTextPalette(PAL0);

	if (gameState->timing.timer < 100)
	{
		VDP_drawText("GLM Studio presents", 5, 10);
		gameState->timing.timer++;
		for (int i = 0; i < gameState->timing.timer / 20; ++i)
		{
			VDP_drawText(".", 5 + i, 11);
		}
		gameplayStarted = 0;
		gameState->second = 1;
	}
	else
	{
		if (gameState->second == 1)
		{
			ClearText();
			gameState->second = 0;
		}

		VDP_drawText("  - T I C   T A C   T O E -", 5, 10);

		VDP_clearText(0, 13, 256);

		VDP_drawText("Level: ", 5, 13);
		{
			if (level == 0)
			{
				VDP_drawText("Easy", 12, 13);
			}
			else if (level == 1)
			{
				VDP_drawText("Medium", 12, 13);
			}
			else if (level == 2)
			{
				VDP_drawText("Hard", 12, 13);
			}

			if (moveUp && canGoUp == 1)
			{
				level++;
				canGoUp = 0;
			}
			else if (moveDown && canGoDown == 1)
			{
				level--;
				canGoDown = 0;
			}
			
			if (moveUp == 0)
			{
				canGoUp = 1;
			}
			if (moveDown == 0)
			{
				canGoDown = 1;
			}

			if (level < 0) level = maxLevel;
			if (level > maxLevel)level = 0;
		}

		VDP_drawText("         Press START", 5, 15);

		if (gameplayStarted == 1)
		{
			gameState->state = 1;
			ClearText();
		}
	}
}

void Init(TTT_GameState* gameState)
{
	JOY_init();
	JOY_setEventHandler(&myJoyHandler);

	SYS_disableInts();

	VDP_setScreenWidth320();
	VDP_setScreenHeight240();

	SPR_init(16, 256, 256);

	SYS_enableInts();

	gameState->posX = gameState->posY = 125;
	gameState->mySprite = NULL;
	gameState->timing.timer = 0;
	gameState->state = MENU;
	gameState->second = 0;
	gameState->isRunning = 1;

	VDP_setBackgroundColor(50);
}

void Update(TTT_GameState* gameState)
{
	if (gameState->state == MENU)
	{
		UpdateMenu(gameState);
	}
	else if (gameState->state == GAMEPLAY)
	{
		UpdateGameplay(gameState);
	}

	SPR_update();
	VDP_waitVSync();
}

void DeInit(TTT_GameState* gameState)
{
	ClearText();

	gameState->posX = gameState->posY = 1;
	gameState->mySprite = NULL;
	gameState->timing.timer = 0;
	gameState->state = MENU;
	gameState->second = 0;
}