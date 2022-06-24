
/*
** GameC engine
** Loop
*/

#ifndef LOOP_H
#define LOOP_H

#include <exec/types.h>

#include "Board.h"

#define ESC_KEY 0x45

enum Events
{
    EID_SAFE_TO_DRAW,
    EID_SAFE_TO_CHANGE,
    EID_USER,
    EID_JOYSTICK,
    EID_COUNT
};

VOID eventLoop(struct screenData *sd, struct windowData *wd, board *board);

#endif /* LOOP_H */
