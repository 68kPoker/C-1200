
/*
** GameX engine
** Src > Bobs
*/

#ifndef BOBS_H
#define BOBS_H

#include <exec/types.h>
#include <exec/nodes.h>

#define MAX_OBJECTS 5 /* Max. number of objects on the board */

#define BOB_SPEED 2  /* Default speed in pixels/frame */

#define TILES 20 /* Tiles in a row */

#define WIDTH 20 /* Board width */
#define HEIGHT 16

#define LEFT    (-1)
#define UP      (-WIDTH)
#define RIGHT   (1)
#define DOWN    (WIDTH)

#define OPPOSITE(dir) (-dir)

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
    BYTE pos, dir;
    struct bobState state, prev[2]; /* Current and previous position */
    BYTE width, height; /* Bob size (16 x 16) */
    WORD speed; /* Movement speed in pixels/frame */
    BOOL update[2]; /* Requires redraw in this buffer (short for state-difference)? */
    BOOL active; /* Process animation? */
    WORD trig; /* Trigger movement (same as direction) */
    VOID (*animate)(struct bobData *bd, struct screenData *sd); /* Custom animation routine */
};

/* initBob: Construct new Bob with initial state and add to list */
VOID initBob(struct bobData *bd, struct List *list, struct BitMap *gfx, WORD gfxX, WORD gfxY, WORD posX, WORD posY, BYTE dir);

VOID clearBG(struct List *list, struct RastPort *rp, WORD frame, struct BitMap *tileGfx, WORD *board);
VOID drawBobs(struct List *list, struct RastPort *rp, WORD frame, struct screenData *sd);
VOID animateBob(struct bobData *bd, struct screenData *sd);

VOID drawTile(struct BitMap *bm, WORD tile, struct RastPort *rp, WORD xpos, WORD ypos);
VOID drawFrame(struct BitMap *bm, struct RastPort *rp, WORD x, WORD y);

#endif /* BOBS_H */
