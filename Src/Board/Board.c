
/*
** GameX engine
** Src > Board
*/

#include <stdio.h>
#include <assert.h>

#include "Board.h"

#define TILES 20

/* Default tiles */
tile tileTypes[T_COUNT] =
{
    { T_NONE, T_NONE },
    { T_NONE, T_BACK },
    { T_NONE, T_WALL },
    { T_NONE, T_FLOOR },
    { T_NONE, T_FLAGSTONE },
    { T_NONE, T_MUD },
    { T_NONE, T_SAND },
    { T_FLOOR, -TID_HERO },
    { T_FLOOR, T_BOX },
    { T_FLOOR, T_SANDBOX }
};

/* Init graphics frames */
VOID constructGfx(WORD *gfxCount, WORD maxCount)
{
    WORD i, prev = 0;

    assert(maxCount >= T_COUNT);

    /* Set graphics frames count */
    gfxCount[T_NONE]       = 0;
    gfxCount[T_BACK]       = 1;
    gfxCount[T_WALL]       = 1;
    gfxCount[T_FLOOR]      = 1;
    gfxCount[T_BOX]        = 1;
    gfxCount[T_FLAGSTONE]  = 1;
    gfxCount[T_SAND]       = 2;    
    gfxCount[T_SANDBOX]    = 1;        
    gfxCount[T_MUD]        = 6;
    gfxCount[T_HERO]       = 1;
/*
    gfxCount[T_BUTTON]     = GC_BUTTON;
    gfxCount[T_TEXT]       = GC_TEXT;
*/
    /* Calc distribution */
    for (i = 0; i < T_COUNT; i++)
    {
        WORD save = gfxCount[i];
        gfxCount[i] = prev;
        prev += save;
    }
}

/* Init */
void constructTile(tile *op, short floor, short object)
{
    op->type  = object;
    op->floor = floor;
}

/* Replace object, return previous object or T_NONE */
short replaceObject(tile *op, short object)
{
    short type = T_NONE;

    if (op->floor == T_NONE)
    {
        op->floor = op->type;
    }
    else
    {
        type = op->type;
    }
    op->type = object;  
    return(type);
}

/* Remove object from tile if present, return its type or T_NONE */
short removeObject(tile *op)
{
    short type = op->type;

    if (op->floor != T_NONE)
    {
        op->type = op->floor;
        return(type);
    }
    return(T_NONE);
}

/* Move object in given direction. Test first before replacing. */
short moveObject(board *bp, tile *op, int offset)
{
    short type = op->type;
    tile *destp = op + offset, *pastp = destp + offset;
    identifiedObject *io;

    switch (type)
    {
    case -TID_HERO:
        switch (destp->type)
        {
        case T_WALL:
            return(0);
        case T_BOX:
            /* Push box */
            if (!moveObject(bp, destp, offset))
            {
                return(0);
            }
            break;
        }
        break;
    case T_BOX:        
        switch (destp->type)
        {
        case T_FLAGSTONE:
            bp->placed++;
        case T_FLOOR:  
            break;
        default:
            return(0);
        }
        switch (op->floor)
        {
        case T_FLAGSTONE:
            bp->placed--;
            break;
        }
        io = bp->objectid + TID_ACTIVE_BOX - 1;

        /* Detach previous first */

        ((tile *)bp->board + io->offset)->type = io->type;        

        io->type = type;
        type = -TID_ACTIVE_BOX;        
        io->offset = op->offset;        

        /* Attach Bob to active box */

        constructBob(&io->bob, bp->gfx, (gfxCount[io->type] % TILES) << 4, (gfxCount[io->type] / TILES) << 4, (io->offset % B_WIDTH) << 4, (io->offset / B_WIDTH) << 4);
        break;
    default:
        return(0);
    }
    /* Move */
    removeObject(op);
    replaceObject(destp, type);
    if (type < 0)
    {
        io = bp->objectid + (-type) - 1;
        io->offset += offset;
        io->pos = 16;
        io->dir = offset;
        io->active = TRUE;        
    }
    return(1);
}

void constructBoard(board *op)
{
    short x, y, offset;
    tile *tp;

    for (y = 0, tp = (tile *)op->board; y < B_HEIGHT; y++)
    {
        for (x = 0; x < B_WIDTH; x++, tp++)
        {
            if (x == 0 || y == 0 || x == B_WIDTH - 1 || y == B_HEIGHT - 1)
            {
                constructTile(tp, T_NONE, T_WALL);
            }
            else
            {
                constructTile(tp, T_NONE, T_FLOOR);
            }
        }
    }

    op->objectid[TID_HERO - 1].offset = offset = getOffset(1, 1);
    replaceObject((tile *)op->board + offset, -TID_HERO);
}

/* Scan board - count boxes and locate hero. Also locate animated floors and objects. */
short scanBoard(board *op)
{
    short x, y;
    tile *tp;
    int hero = 0;

    for (y = 0, tp = (tile *)op->board; y < B_HEIGHT; y++)
    {
        for (x = 0; x < B_WIDTH; x++, tp++)
        {
            if (tp->type == T_BOX)
            {
                op->boxes++;
            }
            else if (tp->type == -TID_HERO)
            {
                op->objectid[TID_HERO - 1].offset = (short)(tp - (tile *)op->board);
                hero = 1;
            }
        }
    }

    if (!hero || op->boxes == 0)
    {
        return(0);
    }
    return(1);
}

/* Bob tile movement (not part of Bob handling) */
VOID animateObject(board *bp, identifiedObject *io)
{    
    if (io->pos > 0)
    {        
        /* Standard movement */
        struct bobData *bd = &io->bob;

        io->pos -= io->speed; /* Update position if moving */        

        io->update[0] = io->update[1] = TRUE;

        switch (io->dir)
        {
            case  Right:  bd->state.posX += io->speed; break;
            case  Left:   bd->state.posX -= io->speed; break;
            case  Down:   bd->state.posY += io->speed; break;
            case  Up:     bd->state.posY -= io->speed; break;
        }
    }    
}

/* Hero animation (not part of Bob handling) */
VOID animateHero(board *bp, identifiedObject *io)
{
    /* Hero animation */

    if (io->pos == 0)
    {
        short trig = io->trig;
        /* Ready for next order */
        if (trig != 0)
        {
            /* Check if movement is possible */
            moveObject(bp, (tile *)bp->board + io->offset, trig);            
        }
    }
    else
    {
        /* Set proper movement animation frame */
        WORD base = gfxCount[T_HERO];

        animateObject(bp, io);
        
        if (io->dir == Right)
        {
            base += 2;
        }
        else if (io->dir == Up)
        {
            base += 4;
        }
        else if (io->dir == Down)
        {
            base += 6;
        }    
        base += (io->pos >> 2) & 0x1;
        changeBob(&io->bob, (base % TILES) << 4, (base / TILES) << 4);
    }
}
