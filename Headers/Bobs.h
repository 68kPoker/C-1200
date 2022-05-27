
/*
** GameX engine
** Src > Bobs
*/

#ifndef BOBS_H
#define BOBS_H

#include <exec/types.h>

#define TILES 20 /* Tiles in a row */

VOID drawTile(struct BitMap *bm, WORD tile, struct RastPort *rp, WORD xpos, WORD ypos);
VOID drawFrame(struct BitMap *bm, struct RastPort *rp, WORD x, WORD y);

#endif /* BOBS_H */
