
/*
** GameX engine
** Src > Bobs
*/

#include <clib/graphics_protos.h>

#include "Bobs.h"

VOID drawTile(struct BitMap *bm, WORD tile, struct windowData *wd, WORD xpos, WORD ypos)
{
    BltBitMapRastPort(bm, (tile % TILES) << 4, (tile / TILES) << 4, wd->w->RPort, xpos, ypos, 16, 16, 0xc0);
}
