
#include "ProgramData.h"
#include "Program.h"

#include "Game.h"
#include "System.h"

long resetProgram(struct programData *pd)
{
    if (resetSystem(&pd->sysData, pd))
    {
        return(resetGame(&pd->gameData, pd));
    }
    return(0);
}
