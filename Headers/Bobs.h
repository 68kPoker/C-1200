
/*
** GameX engine
** Src > Bobs
*/

#ifndef BOBS_H
#define BOBS_H

#include <exec/types.h>
#include <exec/nodes.h>

#define TILES 20 /* Tiles in a row */

#define WIDTH 20 /* Board width */

struct bobData
{
    struct Node node;
    struct BitMap *gfx; /* Graphics data, includes mask bitplane */
    WORD gfxX, gfxY, posX, posY; /* Current graphics source and position (in tiles) */
    BYTE pos, dir; /* Pixel pos relative to tile 0-15, direction in tile address difference */
    WORD width, height; /* Bob size */
    BYTE speed; /* In pixels */
    BOOL update[2]; /* Requires redraw in this buffer? */
    BOOL active; /* Only for animation handler */
    WORD prevPosX[2], prevPosY[2], prevPos[2], prevDir[2]; /* Previous tile and pixel position in both buffers */
    BYTE trig;
    VOID (*animate)(struct bobData *bd, struct screenData *sd);
};

/* Setup new Bob with initial state */
VOID initBob(struct List *list, struct bobData *bd, struct BitMap *gfx, WORD gfxX, WORD gfxY, WORD posX, WORD posY, BYTE dir);

VOID clearBG(struct List *list, struct RastPort *rp, WORD frame, struct BitMap *tileGfx, WORD *board);
VOID drawBobs(struct List *list, struct RastPort *rp, WORD frame);
VOID animateBob(struct bobData *bd, WORD frame);

VOID drawTile(struct BitMap *bm, WORD tile, struct RastPort *rp, WORD xpos, WORD ypos);
VOID drawFrame(struct BitMap *bm, struct RastPort *rp, WORD x, WORD y);

#endif /* BOBS_H */
