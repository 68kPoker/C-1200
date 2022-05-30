
/*
** GameX engine
** Src > Bobs
*/

#include <assert.h>

#include <intuition/intuition.h>

#include <clib/graphics_protos.h>
#include <clib/exec_protos.h>

#include "Bobs.h"
#include "Blitter.h"

#define MAX_OBJECTS 20 /* Max. number of objects on the board */

#define WHITE 2 /* Pen for frames */

#define MAX_TILES (320)

/* Draw tile given by number */
VOID drawTile(struct BitMap *tileGfx, WORD tile, struct RastPort *rp, WORD xpos, WORD ypos)
{
    assert(tile < MAX_TILES);
    assert(xpos >= 0 && xpos <= 304 && ypos >= 0 && ypos <= 240);

    bltBitMapRastPort(tileGfx, (tile % TILES) << 4, (tile / TILES) << 4, rp, xpos & 0xfff0, ypos & 0xfff0, 16, 16, 0xc0);
}

/* Draw tile given by position */
VOID drawBoardTile(struct BitMap *tileGfx, WORD *board, struct RastPort *rp, WORD xpos, WORD ypos)
{
    assert(xpos >= 0 && xpos < WIDTH && ypos >= 0 && ypos < HEIGHT)

    drawTile(tileGfx, board[(ypos * WIDTH) + xpos], rp, xpos << 4, ypos << 4);
}

/* Draw frame around tile */
VOID drawFrame(struct BitMap *bm, struct RastPort *rp, WORD x, WORD y)
{
    SetAPen(rp, WHITE);
    Move(rp, x, y);
    Draw(rp, x + 15, y);
    Draw(rp, x + 15, y + 15);
    Draw(rp, x, y + 15);
    Draw(rp, x, y + 1);
}

/* initBob: Construct new Bob with initial state and add to list */
VOID initBob(struct bobData *bd, struct List *list, struct BitMap *gfx, WORD gfxX, WORD gfxY, WORD posX, WORD posY, BYTE dir)
{
    AddTail(list, &bd->node);

    bd->gfx  = gfx;

    bd->gfxX = gfxX;
    bd->gfxY = gfxY;

    bd->state.posX = posX;
    bd->state.posY = posY;

    bd->dir = dir; /* Direction */
    bd->pos = 0;

    bd->prev[0] = bd->prev[1] = bd->state;

    bd->width  = 16;
    bd->height = 16;

    bd->speed = BOB_SPEED;

    bd->update[0] = bd->update[1] = TRUE; /* This Bob must be drawn */
    bd->active = TRUE; /* This Bob is active */
}

/* Change Bob image */
VOID changeBob(struct bobData *bd, WORD gfxX, WORD gfxY)
{
    if (bd->gfxX != gfxX || bd->gfxY != gfxY)
    {
        bd->gfxX = gfxX;
        bd->gfxY = gfxY;
        
        /* Need to redraw in both buffers */
        bd->update[0] = bd->update[1] = TRUE;
    }
}

/* Trigger movement in given direction. Will start to move when ready. */
VOID triggerBob(struct bobData *bd, WORD dir)
{
    bd->trig = dir;
}

/* Move Bob to new position */
VOID moveBob(struct bobData *bd, WORD posX, WORD posY)
{
    if (bd->state.posX != posX || bd->state.posY != posY)
    {
        bd->state.posX = posX;
        bd->state.posY = posY;

        bd->update[0] = bd->update[1] = TRUE;
    }
}

/* clearTiles: Draw tiles under given Bob position */
VOID clearTiles(struct BitMap *tileGfx, struct RastPort *rp, WORD x, WORD y, WORD *board, WORD *offsets, WORD *n)
{
    WORD tx = x >> 4, ty = y >> 4;
    WORD offset = (ty * WIDTH) + tx;
    WORD *tile = &board[offset];

    if (!(*tile & 0x8000))
    {                
        drawTile(tileGfx, *tile, rp, tx << 4, ty << 4);
        *tile |= 0x8000; /* Flag this tile as drawn */
        offsets[(*n)++] = offset;
    }

    tx = (x + 15) >> 4;
    ty = (y + 15) >> 4;
    offset = (ty * WIDTH) + tx;
    *tile = &board[offset];

    if (!(*tile & 0x8000))
    {
        drawTile(tileGfx, *tile, rp, tx << 4, ty << 4);
        *tile |= 0x8000;
        offsets[(*n)++] = offset;            
    }
}

/* Clear background under Bobs (using tile graphics) */
VOID clearBG(struct List *list, struct RastPort *rp, WORD frame, struct BitMap *tileGfx, WORD *board)
{
    struct Node *node;
    WORD offsets[MAX_OBJECTS * 4];
    WORD i, n = 0;

    for (node = list->lh_Head; node->ln_Succ != NULL; node = node->ln_Succ)
    {
        struct bobData *bd = (struct bobData *)node;

        if (bd->update[frame])
        {
            clearTiles(tileGfx, rp, bd->prev[frame].posX, bd->prev[frame].posY, board, offsets, &n);
            clearTiles(tileGfx, rp, bd->state.posX, bd->state.posY, board, offsets, &n);
        }
    }

    /* Update also those inactive objects that were discarded */
    for (node = list->lh_Head; node->ln_Succ != NULL; node = node->ln_Succ)
    {
        struct bobData *bd = (struct bobData *)node; 
        
        if (board[(bd->state.posY >> 4) * WIDTH + (bd->state.posX >> 4)] & 0x8000)
        {
            bd->update[frame] = TRUE;
        }

        if (board[((bd->state.posY + 15) >> 4) * WIDTH + ((bd->state.posX + 15) >> 4)] & 0x8000)
        {
            bd->update[frame] = TRUE;
        }
    }  

    /* Unflag floors */
    for (i = 0; i < n; i++)
    {
        board[offsets[i]] &= 0x7fff;
    }
}

/* Draw Bob if required */
VOID drawBob(struct bobData *bd, struct RastPort *rp, WORD frame, struct screenData *sd)
{
    WORD x = bd->state.posX, y = bd->state.posY;

    if (!bd->update[frame])
    {
        /* Bob doesn't require update in this frame */
        return;
    }

    /* We assume, background is cleaned up */

    /* Mask is included in source BitMap */
    bltMaskBitMapRastPort(bd->gfx, bd->gfxX, bd->gfxY, rp, x, y, bd->width, bd->height, ABC|ABNC|ANBC);

    bd->prev[frame].posX = x;
    bd->prev[frame].posY = y;
}

/* Draw Bob list */
VOID drawBobs(struct List *list, struct RastPort *rp, WORD frame)
{
    struct Node *node;

    for (node = list->lh_Head; node->ln_Succ != NULL; node = node->ln_Succ)
    {
        drawBob((struct bobData *)node, rp, frame);
    }
}

VOID animateBob(struct bobData *bd, struct screenData *sd)
{
    WORD frame = sd->frame;

    if (bd->animate)
    {
        /* Call custom object animation */
        bd->animate(bd, sd);
    }

    /* Standard movement */
    if (bd->pos == 0)
    {        
        if (bd->trig != 0)
        {
            bd->dir = bd->trig; /* Set direction */
            bd->prevPos[frame] = bd->pos;
            bd->pos = 16;
        }        
    }
    if (bd->pos > 0)
    {        
        bd->pos -= bd->speed; /* Update position if moving */        

        bd->update[0] = bd->update[1] = TRUE;

        switch (bd->dir)
        {
            case  1:     bd->posX += bd->speed; break;
            case -1:     bd->posX -= bd->speed; break;
            case  WIDTH: bd->posY += bd->speed; break;
            case -WIDTH: bd->posY -= bd->speed; break;
        }
    }    
}

VOID animateHero(struct bobData *bd, struct screenData *sd)
{
    /* Hero animation */

    if (bd->pos == 0)
    {
        /* Ready for next order */
    }
    else
    {
        /* Set proper animation frame */
    }
}
