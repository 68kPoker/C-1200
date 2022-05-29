
/*
** GameX engine
** Src > Bobs
*/

#ifndef BOBS_H
#define BOBS_H

#include <exec/types.h>
#include <exec/nodes.h>

#define TILES 20 /* Tiles in a row */

struct bobData
{
    struct Node node;
    struct BitMap *gfx; /* Graphics data, includes mask bitplane */
    WORD gfxX, gfxY, posX, posY; /* Current graphics source and position */
    WORD width, height; /* Bob size */
    BOOL update[2]; /* Requires redraw in this buffer? */
    BOOL active; /* Only for animation handler */
    WORD prevGfxX[2], prevGfxY[2], prevPosX[2], prevPosY[2]; /* Previous state in both buffers */
};

/* Setup new Bob with initial state */
VOID initBob(struct List *list, struct bobData *bd, struct BitMap *gfx, WORD gfxX, WORD gfxY, WORD posX, WORD posY);

VOID clearBG(struct List *list, struct RastPort *rp, WORD frame, struct BitMap *tileGfx, WORD *board);
VOID drawBobs(struct List *list, struct RastPort *rp, WORD frame);

VOID drawTile(struct BitMap *bm, WORD tile, struct RastPort *rp, WORD xpos, WORD ypos);
VOID drawFrame(struct BitMap *bm, struct RastPort *rp, WORD x, WORD y);

#endif /* BOBS_H */
