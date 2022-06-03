
/*
** GameX engine
** Src > Board
*/

#include <stdio.h>
#include <assert.h>

#include <exec/types.h>

#include "Board.h"

#define TILES 20

/* Default tiles */
tile tileTypes[T_COUNT];

WORD gfxCount[T_COUNT];

LONG enterBack(struct sBoard *bp, struct sTile *src, WORD dir);
LONG enterWall(struct sBoard *bp, struct sTile *src, WORD dir);
LONG enterFloor(struct sBoard *bp, struct sTile *src, WORD dir);
LONG enterFlagstone(struct sBoard *bp, struct sTile *src, WORD dir);
LONG enterMud(struct sBoard *bp, struct sTile *src, WORD dir);
LONG enterSand(struct sBoard *bp, struct sTile *src, WORD dir);
LONG enterHero(struct sBoard *bp, struct sTile *src, WORD dir);
LONG enterBox(struct sBoard *bp, struct sTile *src, WORD dir);
LONG enterSandBox(struct sBoard *bp, struct sTile *src, WORD dir);

LONG animateMud(struct sBoard *bp, struct sTile *tile);
LONG animateHero(struct sBoard *bp, struct sTile *tile);
LONG animateBox(struct sBoard *bp, struct sTile *tile);
LONG animateSandBox(struct sBoard *bp, struct sTile *tile);

LONG (*enterTile[T_COUNT])(struct sBoard *bp, struct sTile *src, WORD dir) =
{
    NULL,
    enterBack,
    enterWall,
    enterFloor,
    enterFlagstone,
    enterMud,
    enterSand,
    enterHero,
    enterBox,
    enterSandBox
};

LONG (*animateTile[T_COUNT])(struct sBoard *bp, struct sTile *tile) = 
{ 
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    animateMud,
    NULL,
    animateHero,
    animateBox,
    animateSandBox
};

/* Construct new type */
VOID constructType(struct sTile *tile, WORD floor, WORD type)
{
    tile->floor = floor;
    tile->type = type;
    tile->floorID = TID_NONE;
    tile->typeID = TID_NONE;
}

VOID constructTypes(struct sTile *tileTypes, WORD maxCount)
{
    assert(maxCount >= T_COUNT);

    constructType(tileTypes + T_NONE, T_NONE, T_NONE);
    constructType(tileTypes + T_BACK, T_NONE, T_BACK);
    constructType(tileTypes + T_WALL, T_NONE, T_WALL);
    constructType(tileTypes + T_FLOOR, T_NONE, T_FLOOR);
    constructType(tileTypes + T_FLAGSTONE, T_NONE, T_FLAGSTONE);
    constructType(tileTypes + T_MUD, T_NONE, T_MUD);
    constructType(tileTypes + T_SAND, T_NONE, T_SAND);
    constructType(tileTypes + T_HERO, T_FLOOR, T_HERO);
    constructType(tileTypes + T_BOX, T_FLOOR, T_BOX);
    constructType(tileTypes + T_SANDBOX, T_FLOOR, T_SANDBOX);
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

/* Replace object */
void replaceObject(tile *op, short object, short objectID)
{
    if (op->floor == T_NONE)
    {
        /* Save floor info */
        op->floor = op->type;
        op->floorID = op->typeID;
    }
    op->type = object;
    op->typeID = objectID;
    return(type);
}

/* Remove object from tile if present */
void removeObject(tile *op)
{
    if (op->floor != T_NONE)
    {
        op->type = op->floor;
        op->typeID = op->floorID;
    }
}

/* Move object in given direction */
short moveObject(struct sBoard *bp, struct sTile *op, int dir)
{
    struct sTile *destp = op + dir;
    struct sIdentifiedObject *io;

    replaceObject(destp, type);
    removeObject(op);

    if (destp->type == T_BOX || destp->type == T_SANDBOX)
    {
        /* Mark as active box */        

        destp->typeID = TID_ACTIVE_BOX;
        io = &bp->objectData[TID_ACTIVE_BOX - 1];
        
        io->offset = (short)(destp - (struct sTile *)bp->board);

        /* TODO: Attach Bob to active box */;
    }

    /* Set move info */
    io = bp->objectData + destp->typeID - 1;
    io->offset += dir;
    io->pos = 16;
    io->dir = dir;
    io->active = TRUE;        
    
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
                *tp = tileTypes[T_WALL];                
            }
            else
            {
                *tp = tileTypes[T_FLOOR];                
            }
        }
    }

    op->objectid[TID_HERO - 1].offset = offset = getOffset(1, 1);
    replaceObject((tile *)op->board + offset, T_HERO, TID_HERO);
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
            else if (tp->type == T_HERO)
            {
                if (!hero)
                {
                    /* Put ID */
                    tp->typeID = TID_HERO;

                    /* Store position */
                    op->objectData[TID_HERO - 1].offset = (y * B_WIDTH) + x;

                    /* Hero found */
                    hero = 1;
                }
                else
                {
                    /* Multiple heroes */
                    return(0);
                }
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

LONG enterBack(struct sBoard *bp, struct sTile *src, WORD dir)
{
    return(FALSE);
}

LONG enterWall(struct sBoard *bp, struct sTile *src, WORD dir)
{
    return(FALSE);
}

LONG enterFloor(struct sBoard *bp, struct sTile *src, WORD dir)
{
    moveObject(bp, src, dir);
    return(TRUE);
}

LONG enterFlagstone(struct sBoard *bp, struct sTile *src, WORD dir)
{
    struct sTile *dest = src + dir;

    if (src->type == T_BOX)
    {
        if (src->floor != T_FLAGSTONE)
        {
            bp->placed++;
        }
    }
    moveObject(bp, src, dir);
    return(TRUE);
}

LONG enterMud(struct sBoard *bp, struct sTile *src, WORD dir)
{
    if (src->type == T_SANDBOX)
    {
        /* TODO: Change floor to sand, remove sandbox */
        moveObject(bp, src, dir);
      
        return(TRUE);
    }

    return(FALSE);
}

LONG enterSand(struct sBoard *bp, struct sTile *src, WORD dir)
{
    moveObject(bp, src, dir);
    return(TRUE);
}

LONG enterHero(struct sBoard *bp, struct sTile *src, WORD dir)
{
    return(FALSE);
}

LONG enterBox(struct sBoard *bp, struct sTile *src, WORD dir)
{
    struct sTile *dest = src + dir, *next = dest + dir;

    if (src->type == T_HERO)
    {
        if (enterTile[next->type](bp, dest, dir))
        {
            moveObject(bp, src, dir);
            return(TRUE);
        }
    }

    return(FALSE);
}

LONG enterSandBox(struct sBoard *bp, struct sTile *src, WORD dir)
{
    struct sTile *dest = src + dir, *next = dest + dir;

    if (src->type == T_HERO)
    {
        if (enterTile[next->type](bp, dest, dir))
        {
            moveObject(bp, src, dir);
            return(TRUE);
        }
    }

    return(FALSE);
}

LONG animateMud(struct sBoard *bp, struct sTile *tile)
{
    
}

LONG animateHero(struct sBoard *bp, struct sTile *tile)
{
    /* Handle standard movement if pos > 0 */

    /* Handle trigger when pos == 0 */
}

LONG animateBox(struct sBoard *bp, struct sTile *tile)
{

}

LONG animateSandBox(struct sBoard *bp, struct sTile *tile)
{
    
}
