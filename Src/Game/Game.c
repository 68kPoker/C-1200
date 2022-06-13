
#include "GameData.h"
#include "Game.h"

#include "Board.h"

long resetGame(struct gameData *gd, struct programData *pd)
{
    return(resetBoard(&gd->boardData, gd, pd));
}
