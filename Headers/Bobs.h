
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

#define TID(tile) (tile & 0x00ff) /* Obtain tile/object */

#define setOBJ(floor, obj) (floor = ((floor & 0x00ff) << 8) | (obj & 0x00ff)) /* Add floor info */

#define remOBJ(floor) (floor = ((floor >> 8) & 0x00ff))

#define FLOOR(tile) ((tile >> 8) & 0x00ff)

enum
{
	OID_HERO,
	OID_BOX
};	

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
    WORD tileOffset; /* Tile position */
    BYTE pos, dir;
    struct bobState state, prev[2]; /* Current and previous position */
    BYTE width, height; /* Bob size (16 x 16) */
    WORD speed; /* Movement speed in pixels/frame */
    BOOL update[2]; /* Requires redraw in this buffer (short for state-difference)? */
    BOOL active; /* Process animation? */
    BYTE trig, repeat; /* Trigger movement (same as direction) */
    VOID (*animate)(struct bobData *bd, struct screenData *sd, WORD *board); /* Custom animation routine */
    WORD type;
};

/* initBob: Construct new Bob with initial state and add to list */
VOID initBob(struct bobData *bd, struct List *list, struct BitMap *gfx, WORD gfxX, WORD gfxY, WORD posX, WORD posY, BYTE dir);

VOID clearBG(struct List *list, struct RastPort *rp, WORD frame, struct screenData *sd, WORD *board);
VOID drawBobs(struct List *list, struct RastPort *rp, WORD frame, struct screenData *sd, WORD *board);
VOID animateBob(struct bobData *bd, struct screenData *sd, WORD *board);

VOID drawTile(struct screenData *sd, WORD tile, struct RastPort *rp, WORD xpos, WORD ypos, BOOL floorOnly);
VOID drawFrame(struct RastPort *rp, WORD x, WORD y);

VOID animateHero(struct bobData *bd, struct screenData *sd, WORD *board);

#endif /* BOBS_H */
