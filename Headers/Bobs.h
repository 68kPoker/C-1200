
/*
** GameX engine
** Src > Bobs
*/

#ifndef BOBS_H
#define BOBS_H

#include <exec/types.h>
#include <exec/nodes.h>

#define MAX_BOBS    10

#if 0
#define BOB_SPEED 2  /* Default speed in pixels/frame */
#endif

/* Bob state Class */
struct bobState
{
    WORD posX, posY; /* Bitmap position (in pixels) */
};

/* Bob Class */
struct bobData
{
    struct Node node;
    struct BitMap *gfx; /* Graphics data, includes mask bitplane */
    WORD gfxX, gfxY;    
    struct bobState state, prev[2]; /* Current and previous position */
    BYTE width, height; /* Bob size (16 x 16) */    
    BOOL update[2]; /* Requires redraw in this buffer (short for state-difference)? */
    BOOL active; /* Process animation? */
};

/* Construct new Bob with initial state */
VOID constructBob(struct bobData *bd, struct BitMap *gfx, WORD gfxX, WORD gfxY, WORD posX, WORD posY);
VOID easyConstructBob(struct bobData *bd, struct BitMap *gfx, WORD gfx, WORD pos);

VOID clearBG(struct List *list, struct RastPort *rp, WORD frame, struct screenData *sd, struct sBoard *board);
VOID drawBobs(struct List *list, struct RastPort *rp, WORD frame, struct screenData *sd, struct sBoard *board);
VOID animateBob(struct bobData *bd, struct screenData *sd, struct sBoard *board);

VOID drawTile(struct screenData *sd, struct sTile *tile, struct RastPort *rp, WORD xpos, WORD ypos, BOOL floorOnly);
VOID drawFrame(struct RastPort *rp, WORD x, WORD y);

#endif /* BOBS_H */
