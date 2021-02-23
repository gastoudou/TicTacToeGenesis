#ifndef __GAME_STATE__
#define __GAME_STATE__

#include <genesis.h>

typedef struct
{
	u16 timer;
} Timing;

typedef struct
{
	u32 posX;
	u32 posY;
	Timing timing;
	Sprite* mySprite;
	u8 state;
	u8 second;
	u8 isRunning;

} TTT_GameState;

void AITurn();
void TryClic(u32 x, u32 y);
void myJoyHandler(u16 joy, u16 changed, u16 state);
void ClearText();
void UpdateGameplay(TTT_GameState* gameState);
void UpdateMenu(TTT_GameState* gameState);
void Init(TTT_GameState* gameState);
void Update(TTT_GameState* gameState);
void DeInit(TTT_GameState* gameState);

#endif // !__GAME_STATE__