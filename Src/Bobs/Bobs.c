
/*
** GameX engine
** Src > Bobs
*/

#include <intuition/intuition.h>

#include <clib/graphics_protos.h>

#include "Bobs.h"
#include "Blitter.h"

#define WHITE 2

VOID drawTile(struct BitMap *bm, WORD tile, struct RastPort *rp, WORD xpos, WORD ypos)
{
    bltBitMapRastPort(bm, (tile % TILES) << 4, (tile / TILES) << 4, rp, xpos, ypos, 16, 16, 0xc0);
}

VOID drawFrame(struct BitMap *bm, struct RastPort *rp, WORD x, WORD y)
{
    SetAPen(rp, WHITE);
    Move(rp, x, y);
    Draw(rp, x + 15, y);
    Draw(rp, x + 15, y + 15);
    Draw(rp, x, y + 15);
    Draw(rp, x, y + 1);
}
