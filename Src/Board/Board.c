
/*
** GameX engine
** Src > Board
*/

#include <stdio.h>
#include <assert.h>
#include "debug.h"

#include <exec/types.h>

#include "Board.h"

#define TILES 20

/* Default tiles */
tile tileTypes[T_COUNT];

WORD gfxCount[T_COUNT];

LONG actionBack(struct sBoard *bp, struct sTile *src, WORD dir, WORD action);
LONG actionWall(struct sBoard *bp, struct sTile *src, WORD dir, WORD action);
LONG actionFloor(struct sBoard *bp, struct sTile *src, WORD dir, WORD action);
LONG actionFlagstone(struct sBoard *bp, struct sTile *src, WORD dir, WORD action);
LONG actionMud(struct sBoard *bp, struct sTile *src, WORD dir, WORD action);
LONG actionSand(struct sBoard *bp, struct sTile *src, WORD dir, WORD action);
LONG actionHero(struct sBoard *bp, struct sTile *src, WORD dir, WORD action);
LONG actionBox(struct sBoard *bp, struct sTile *src, WORD dir, WORD action);
LONG actionSandBox(struct sBoard *bp, struct sTile *src, WORD dir, WORD action);

LONG animateMud(struct sBoard *bp, struct sTile *tile);
LONG animateHero(struct sBoard *bp, struct sTile *tile);
LONG animateBox(struct sBoard *bp, struct sTile *tile);
LONG animateSandBox(struct sBoard *bp, struct sTile *tile);

enum /* Actions */
{
    KNOCK, /* Check movement */
    MOVE, /* Move object */
    LEAVE, /* Leave tile */
    ENTER, /* Enter tile */
    ENTERED /* Movement done */
};

LONG (*actionTile[T_COUNT])(struct sBoard *bp, struct sTile *src, WORD dir, WORD action) =
{
    NULL,
    actionBack,
    actionWall,
    actionFloor,
    actionBox,
    actionFlagstone,
    actionSand,
    actionSandBox,
    actionMud,
    actionHero
};

LONG (*animateTile[T_COUNT])(struct sBoard *bp, struct sTile *tile) = 
{ 
    NULL,
    NULL,
    NULL,
    NULL,
    animateBox,
    NULL,
    NULL,
    animateSandBox,
    animateMud,
    animateHero
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
    gfxCount[T_MUD]        = 6 + 16;
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
}

/* Remove object from tile if present */
void removeObject(tile *op)
{
    if (op->floor != T_NONE)
    {
        op->type = op->floor;
        op->typeID = op->floorID;

        op->floor = T_NONE;
    }
}

/* Move object in given direction */
short moveObject(struct sBoard *bp, struct sTile *op, int dir)
{
    struct sTile *destp = op + dir;
 
    /* Check if movement is possible */
    if (actionTile[destp->type](bp, op, dir, KNOCK))
    {
        actionTile[op->floor](bp, op, dir, LEAVE);
        actionTile[op->type](bp, op, dir, MOVE);
        actionTile[destp->floor](bp, op, dir, ENTER);

        replaceObject(destp, op->type, op->typeID);
        removeObject(op);

        if (destp->typeID > 0)
        {
            struct sIdentifiedObject *io = bp->objectData + destp->typeID - 1;

            /* Set move info */ 
            io->offset += dir;
            io->pos = 16;
            io->dir = dir;
            io->speed = 1;
            io->active = TRUE;  
        }
        return(1);
    }
    return(0);
}


    if (op->type == T_BOX || op->type == T_SANDBOX)
    {

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
                *tp = tileTypes[T_WALL];                
            }
            else
            {
                *tp = tileTypes[T_FLOOR];                
            }
        }
    }

    op->objectData[TID_HERO - 1].offset = offset = getOffset(1, 1);
    replaceObject((tile *)op->board + offset, T_HERO, TID_HERO);
    
    D(bug("TypeID = %d\n", ((tile *)op->board + offset)->typeID));
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

        io->bob.update[0] = io->bob.update[1] = TRUE;

        switch (io->dir)
        {
            case  Right:  bd->state.posX += io->speed; break;
            case  Left:   bd->state.posX -= io->speed; break;
            case  Down:   bd->state.posY += io->speed; break;
            case  Up:     bd->state.posY -= io->speed; break;
        }
        if (io->pos == 0)
        {
            /* Movement done */
            actionTile[(tile *)bp->board + io->offset](bp, (tile *)bp->board + io->offset, io->dir, ENTERED);
        }
    }    
}

LONG actionBack(struct sBoard *bp, struct sTile *src, WORD dir, WORD action)
{
    if (action == KNOCK)
    {    
        return(FALSE);
    }
}

LONG actionWall(struct sBoard *bp, struct sTile *src, WORD dir, WORD action)
{
    if (action == KNOCK)
    {
        return(FALSE);
    }
}

LONG actionFloor(struct sBoard *bp, struct sTile *src, WORD dir, WORD action)
{
    if (action == KNOCK)
    {
        return(TRUE);
    }
}

LONG actionFlagstone(struct sBoard *bp, struct sTile *src, WORD dir, WORD action)
{
    struct sTile *dest = src + dir;

    if (action == KNOCK)
    {
        return(TRUE);
    }
    else if (action == ENTER)
    {
        /* Enter flagstone */
        if (src->type == T_BOX)
        {  
            bp->placed++;
        }            
    }
    else if (action == LEAVE)
    {
        /* Leave flagstone */
        if (src->type == T_BOX)
        {
            bp->placed--;
        }
    }
}

LONG actionMud(struct sBoard *bp, struct sTile *src, WORD dir, WORD action)
{
    if (action == KNOCK)
    {
        if (src->type != T_SANDBOX)
        {
            return(FALSE);
        }
        return(TRUE);
    }
    else if (action == ENTERED)
    {
        /* TODO: Change mud (floor) to sand, remove sandbox */
    }
}

LONG actionSand(struct sBoard *bp, struct sTile *src, WORD dir, WORD action)
{
    if (action == KNOCK)
    {
        return(TRUE);
    }    
}

LONG actionHero(struct sBoard *bp, struct sTile *src, WORD dir, WORD action)
{
    if (action == KNOCK)
    {    
        return(FALSE);
    }
    else if (action == MOVE)
    {
        if ((src + dir)->floor != T_NONE)
        {
            /* Push object */
            moveObject(bp, src + dir, dir);
        }
    }
}

LONG actionBox(struct sBoard *bp, struct sTile *src, WORD dir, WORD action)
{
    struct sTile *dest = src + dir, *next = dest + dir;

    if (action == KNOCK)
    {        
        if (src->type == T_HERO)
        {
            return (actionTile[next->type](bp, dest, dir, KNOCK));            
        }
        return(FALSE);
    }
    else if (action == MOVE)
    {
        /* Mark as active box */        

        struct identifiedObject *io;
        
        src->typeID = TID_ACTIVE_BOX;
        io = &bp->objectData[TID_ACTIVE_BOX - 1];        
        
        io->offset = (short)((tile *)src - bp->board); /* Box offset */
        
        easyConstructBob(&io->bob, bp->gfx, gfxCount[src->type], io->offset);        
    }
}

LONG actionSandBox(struct sBoard *bp, struct sTile *src, WORD dir)
{
    struct sTile *dest = src + dir, *next = dest + dir;

    if (action == KNOCK)
    {
        if (src->type == T_HERO)
        {
            return (actionTile[next->type](bp, dest, dir, KNOCK));                                
        }
        return(FALSE);
    }
    else if (action == MOVE)
    {
        /* Mark as active box */        

        struct identifiedObject *io;
        
        src->typeID = TID_ACTIVE_BOX;
        io = &bp->objectData[TID_ACTIVE_BOX - 1];        
        
        io->offset = (short)((tile *)src - bp->board); /* Box offset */
        
        easyConstructBob(&io->bob, bp->gfx, gfxCount[src->type], io->offset);        
    }
}

LONG animateMud(struct sBoard *bp, struct sTile *tile)
{
    
}

/* Hero animation */
LONG animateHero(struct sBoard *bp, struct sTile *tile)
{
    struct sIdentifiedObject *io;
    
    if (tile->typeID == 0)
    {
        return;
    }
    
    io = bp->objectData + tile->typeID - 1;  
    
    /* Handle trigger when pos == 0 */ 
    if (io->pos == 0)
    {
        short trig = io->trig;
        
        /* Ready for next order */
        if (trig != 0)
        {
            struct sTile *dest = tile + trig;
            /* Check if movement is possible */
            if (moveObject(bp, tile, trig))
            {
                animateObject(bp, io);
            }
        }
    }
    else
    {
        /* Handle standard movement if pos > 0 */
        /* Set proper movement animation frame */
        WORD base = gfxCount[T_HERO];

        animateObject(bp, io);
        
        if (io->dir == Right)
        {
            base += 2;
        }
        else if (io->dir == Down)
        {
            base += 4;
        }
        else if (io->dir == Up)
        {
            base += 6;
        }    
        base += (io->pos >> 2) & 0x1;
        changeBob(&io->bob, (base % TILES) << 4, (base / TILES) << 4);
    }
}

LONG animateBox(struct sBoard *bp, struct sTile *tile)
{
    struct sIdentifiedObject *io;
    
    if (tile->typeID)
    {
        io = bp->objectData + tile->typeID - 1;    
    
        animateObject(bp, io);
    }    
}

LONG animateSandBox(struct sBoard *bp, struct sTile *tile)
{

}
