
/*
** GameX engine
** Src > Bobs
*/

#ifndef BOBS_H
#define BOBS_H

#include <exec/types.h>

#define TILES 20 /* Tiles in a row */

VOID drawTile(struct BitMap *bm, WORD tile, struct windowData *wd, WORD xpos, WORD ypos);

#endif /* BOBS_H */
