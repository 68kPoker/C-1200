
/*
** GameX engine
** Src > Bobs
*/

#include <stdio.h>
#include <assert.h>
#include "debug.h"

#include <intuition/intuition.h>

#include <clib/graphics_protos.h>
#include <clib/exec_protos.h>

#include "Bobs.h"
#include "Blitter.h"
#include "Screen.h"
#include "Edit.h"
#include "Board.h"

#define WHITE 2 /* Pen for frames */

#define MAX_TILES (320)

/* Draw tile given by number */
VOID drawTile(struct screenData *sd, tile *tile, struct RastPort *rp, WORD xpos, WORD ypos, BOOL floorOnly)
{
    struct BitMap *tileGfx = sd->gfx;

    assert(tile < MAX_TILES);
    assert(xpos >= 0 && xpos <= 304 && ypos >= 0 && ypos <= 240);

    /* TODO: Dual-layer tiles, animation frames */

    if (tile->floor == T_NONE)
    {
        /* Floor only */
        WORD gfx = gfxCount[tile->type];
        bltBitMapRastPort(tileGfx, (gfx % TILES) << 4, (gfx / TILES) << 4, rp, xpos & 0xfff0, ypos & 0xfff0, 16, 16, 0xc0);
    }
    else
    {
        /* Floor + object */
        WORD gfx = gfxCount[tile->floor];
        bltBitMapRastPort(tileGfx, (gfx % TILES) << 4, (gfx / TILES) << 4, rp, xpos & 0xfff0, ypos & 0xfff0, 16, 16, 0xc0);
        if (!floorOnly)
        {
            gfx = gfxCount[tile->type];
            bltMaskBitMapRastPort(tileGfx, (gfx % TILES) << 4, (gfx / TILES) << 4, rp, xpos & 0xfff0, ypos & 0xfff0, 16, 16, ABC|ABNC|ANBC, sd->mask);
        }
    }
}

/* Draw tile given by position */
VOID drawBoardTile(struct screenData *sd, board *board, struct RastPort *rp, WORD xpos, WORD ypos, BOOL floorOnly)
{
    assert(xpos >= 0 && xpos < WIDTH && ypos >= 0 && ypos < HEIGHT)

    drawTile(sd, (tile *)board->board + (ypos * WIDTH) + xpos, rp, xpos << 4, ypos << 4, floorOnly);
}

/* Draw frame around tile */
VOID drawFrame(struct RastPort *rp, WORD x, WORD y)
{
    SetAPen(rp, WHITE);
    Move(rp, x, y);
    Draw(rp, x + 15, y);
    Draw(rp, x + 15, y + 15);
    Draw(rp, x, y + 15);
    Draw(rp, x, y + 1);
}

/* initBob: Construct new Bob with initial state and add to list */
VOID constructBob(struct bobData *bd, struct BitMap *gfx, WORD gfxX, WORD gfxY, WORD posX, WORD posY)
{
    bd->gfx  = gfx;

    bd->gfxX = gfxX;
    bd->gfxY = gfxY;

    bd->state.posX = posX;
    bd->state.posY = posY;

    bd->prev[0] = bd->prev[1] = bd->state;

    bd->width  = 16;
    bd->height = 16;

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
VOID clearTiles(struct screenData *sd, struct RastPort *rp, WORD x, WORD y, board *board, WORD *offsets, WORD *n)
{
    WORD tx = x >> 4, ty = y >> 4;
    WORD offset = (ty * WIDTH) + tx;
    tile *tile = (tile *)board->board + offset;

    if (!tile->flag)
    {                
        drawTile(sd, tile, rp, tx << 4, ty << 4, TRUE);
        tile->flag = 1; /* Flag this tile as drawn */
        offsets[(*n)++] = offset; /* Remember tile offset */
    }

    tx = (x + 15) >> 4;
    ty = (y + 15) >> 4;
    offset = (ty * WIDTH) + tx;
    tile = (tile *)board->board + offset;

    if (!tile->flag)
    {
        drawTile(sd, tile, rp, tx << 4, ty << 4, TRUE);
        tile->flag = 1;
        offsets[(*n)++] = offset;            
    }
}

/* Clear background under Bobs (using tile graphics) */
VOID clearBG(struct List *list, struct RastPort *rp, WORD frame, struct screenData *sd, board *board)
{
    struct Node *node;
    WORD offsets[MAX_OBJECTS * 4];
    WORD i, n = 0;

    for (node = list->lh_Head; node->ln_Succ != NULL; node = node->ln_Succ)
    {
        struct bobData *bd = (struct bobData *)node;

        if (bd->update[frame])
        {
            clearTiles(sd, rp, bd->prev[frame].posX, bd->prev[frame].posY, board, offsets, &n);
            clearTiles(sd, rp, bd->state.posX, bd->state.posY, board, offsets, &n);
        }
    }

    /* Update also those inactive objects that were discarded */
    for (node = list->lh_Head; node->ln_Succ != NULL; node = node->ln_Succ)
    {
        struct bobData *bd = (struct bobData *)node; 
        
        if (board->board[bd->state.posY >> 4][bd->state.posX >> 4].flag)
        {
            bd->update[frame] = TRUE;
        }

        if (board->board[(bd->state.posY + 15) >> 4][(bd->state.posX + 15) >> 4].flag)
        {
            bd->update[frame] = TRUE;
        }
    }  

    /* Unflag floors */
    for (i = 0; i < n; i++)
    {
        ((tile *)board->board + offsets[i])->flag = 0;
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
    bltMaskBitMapRastPort(bd->gfx, bd->gfxX, bd->gfxY, rp, x, y, bd->width, bd->height, ABC|ABNC|ANBC, sd->mask);

    bd->update[frame] = FALSE;

    bd->prev[frame].posX = x;
    bd->prev[frame].posY = y;
}

/* Draw Bob list */
VOID drawBobs(struct List *list, struct RastPort *rp, WORD frame, struct screenData *sd, board *board)
{
    struct Node *node;
    
    clearBG(list, rp, frame, sd, board);
    
    for (node = list->lh_Head; node->ln_Succ != NULL; node = node->ln_Succ)
    {
        if (((struct bobData *)node)->active)
        {
            drawBob((struct bobData *)node, rp, frame, sd);        
        }    
    }

    for (node = list->lh_Head; node->ln_Succ != NULL; node = node->ln_Succ)
    {
        if (((struct bobData *)node)->active)
        {
            animateBob((struct bobData *)node, sd, board);
        }   
    }
}

VOID animateBob(struct bobData *bd, struct screenData *sd, board *board)
{
    if (bd->animate)
    {
        /* Call custom object animation */
        bd->animate(bd, sd, board);
    }
}

/* Bob tile movement (not part of Bob handling) */
#if 0
{
    /* Standard movement */
    if (bd->pos == 0)
    {        
        if (bd->trig != 0)
        {
            bd->dir = bd->trig; /* Set direction */
            bd->pos = 16;
            bd->tileOffset += bd->dir;
            
            if (!bd->repeat)
            {
                bd->trig = 0;
            }   
        }        
    }
    if (bd->pos > 0)
    {        
        bd->pos -= bd->speed; /* Update position if moving */        

        bd->update[0] = bd->update[1] = TRUE;

        switch (bd->dir)
        {
            case  1:     bd->state.posX += bd->speed; break;
            case -1:     bd->state.posX -= bd->speed; break;
            case  WIDTH: bd->state.posY += bd->speed; break;
            case -WIDTH: bd->state.posY -= bd->speed; break;
        }
        
        if (bd->pos == 0)
        {
            WORD tile = board[bd->tileOffset]; 
        
            if (TID(tile) == TID_SAND_BOX && FLOOR(tile) == TID_MUD)
            {
                bd->active = FALSE;
                board[bd->tileOffset] = TID_GRASS | (TID_GRASS << 8);
            }
            else if (TID(tile) == TID_WATER_BOX && FLOOR(tile) == TID_FIRE)
            {
                bd->active = FALSE;
                board[bd->tileOffset] = TID_GRASS | (TID_GRASS << 8);
            }    
        }
    }    
}
#endif

/* Hero animation (not part of Bob handling) */

#if 0
VOID animateHero(struct bobData *bd, struct screenData *sd, board *board)
{
    /* Hero animation */

    if (bd->pos == 0)
    {
        /* Ready for next order */
        if (bd->trig != 0)
        {
            /* Check if movement is possible */
            WORD cur = bd->tileOffset, dest = cur + bd->trig, past = dest + bd->trig;
            
            bd->speed = 2;
            
            if (TID(board[dest]) == TID_WALL || TID(board[dest]) == TID_BACK)
            {
                /* Stop hero */
                bd->trig = 0;
            }
            else if (TID(board[dest]) == TID_KEY_BOX    ||
                     TID(board[dest]) == TID_SAND_BOX   ||
                     TID(board[dest]) == TID_WATER_BOX  ||
                     TID(board[dest]) == TID_BOMB_BOX   ||
                     TID(board[dest]) == TID_SKULL)
            {
                if (TID(board[past]) == TID_FLOOR       || 
                    TID(board[past]) == TID_KEY_STONE   ||
                    TID(board[past]) == TID_MUD         ||
                    TID(board[past]) == TID_GRASS       ||
                    TID(board[past]) == TID_FIRE)
                {
                    initBob(sd->bob + 1, &sd->bobs, sd->gfx, TID(board[dest]) << 4, 0, (dest % WIDTH) << 4, (dest / WIDTH) << 4, bd->trig);
                    sd->bob[1].trig = bd->trig;
                    sd->bob[1].repeat = FALSE;
                    bd->speed = 1;

                    setOBJ(board[past], board[dest]);
                    remOBJ(board[dest]); /* Put hero object */
                }
                else
                {
                    bd->trig = 0;
                }   
            }
        }
    }
    else
    {
        /* Set proper movement animation frame */
        
        if (bd->dir == -1)
        {
            changeBob(bd, (10 + ((bd->pos >> 2) & 0x1)) << 4, 16);
        }
        else if (bd->dir == 1)
        {
            changeBob(bd, (12 + ((bd->pos >> 2) & 0x1)) << 4, 16);
        }
        else if (bd->dir == WIDTH)
        {
            changeBob(bd, (14 + ((bd->pos >> 2) & 0x1)) << 4, 16);
        }
        else if (bd->dir == -WIDTH)
        {
            changeBob(bd, (16 + ((bd->pos >> 2) & 0x1)) << 4, 16);
        }    
    }
}
#endif
