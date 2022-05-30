
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

#define MAX_OBJECTS 20

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

VOID initBob(struct List *list, struct bobData *bd, struct BitMap *gfx, WORD gfxX, WORD gfxY, WORD posX, WORD posY, BYTE dir)
{
    AddTail(list, &bd->node);

    bd->gfx = gfx;
    bd->gfxX = gfxX;
    bd->gfxY = gfxY;
    bd->posX = bd->prevPosX[0] = bd->prevPosX[1] = posX;
    bd->posY = bd->prevPosY[0] = bd->prevPosY[1] = posY;

    bd->width = 16;
    bd->height = 16;

    bd->speed = 2; /* Default speed */

    bd->dir = dir; /* Direction */
    bd->pos = 0;

    bd->update[0] = bd->update[1] = TRUE; /* This Bob must be drawn */
    bd->active = TRUE; /* This Bob is active */
}

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
            clearTiles(tileGfx, rp, bd->prevPosX[frame], bd->prevPosY[frame], board, offsets, &n);
            clearTiles(tileGfx, rp, bd->posX[frame], bd->posY[frame], board, offsets, &n);
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
    WORD x = bd->posX, y = bd->posY;
    if (!bd->update[frame])
    {
        /* Bob doesn't require update in this frame */
        return;
    }

    animateBob(bd, sd);

    /* We assume, background is cleaned up */

    /* Mask is included in source BitMap */
    bltMaskBitMapRastPort(bd->gfx, bd->gfxX, bd->gfxY, rp, x, y, bd->width, bd->height, ABC|ABNC|ANBC);

    bd->prevPosX[frame] = x;
    bd->prevPosY[frame] = y;
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

    /* Standard movement */
    if (bd->pos == 0)
    {        
        if (bd->animate)
        {
            /* Call custom object animation */
            bd->animate(bd, sd);
        }

        if (bd->trig != 0)
        {
            bd->dir = bd->trig; /* Set direction */
            bd->prevPos[frame] = bd->pos;
            bd->pos = 16;
        }        
    }
    if (bd->pos > 0)
    {
        bd->prevPos[frame] = bd->pos;
        bd->pos -= bd->speed; /* Update position if moving */        

        switch (bd->dir)
        {
            case  1:     bd->posX += bd->speed; break;
            case -1:     bd->posX -= bd->speed; break;
            case  WIDTH: bd->posY += bd->speed; break;
            case -WIDTH: bd->posY -= bd->speed; break;
        }
    }    
}

VOID moveBob(struct bobData *bd, WORD dir)
{
    bd->trig = dir;
}

VOID animateHero(struct bobData *bd, struct screenData *sd)
{

}
