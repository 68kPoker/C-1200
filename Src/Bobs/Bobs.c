
/*
** GameX engine
** Src > Bobs
*/

#include <intuition/intuition.h>

#include <clib/graphics_protos.h>

#include "Bobs.h"

VOID drawTile(struct BitMap *bm, WORD tile, struct RastPort *rp, WORD xpos, WORD ypos)
{
    BltBitMapRastPort(bm, (tile % TILES) << 4, (tile / TILES) << 4, rp, xpos, ypos, 16, 16, 0xc0);
}
