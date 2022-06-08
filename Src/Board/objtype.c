
/* 
** Obj_types.c
*/

#include "objtype.h"
#include "floortype.h"

#define DISTANCE 16
#define SPEED 2

#define objectStill(pos) pos == 0
#define objectOrder(order) order != 0
#define getNext(cur, order) cur + order

void calcObjTypeGfx(int *gfx)
{
    int counter = 0;

    counter = setCount(gfx, NONE, counter, 0);
    counter = setCount(gfx, HERO, counter, 1);
    counter = setCount(gfx, BOX, counter, 1);
}

int canMove(struct tile *cur, int type, int order)
{    
    struct tile *next = getNext(cur, order);

    switch (type)
    {
        case HERO:
            if (next->obj != NONE && canMove(next->obj, order))
            {
                return(TRUE);
            }            
            else if (next->obj == NONE && next->floor == WALL)
            {
                return(FALSE);
            }
            return(TRUE);            
        case BOX:
            if (next->obj == NONE && next->floor != WALL)
            {
                return(TRUE);
            }
            return(FALSE);
    }
    return(FALSE);
}

void moveObject(struct tile *cur, int order)
{
    struct tile *next = getNext(cur, order);

    if (next->obj)
    {
        /* Push */
        struct tile *past = getNext(next, order);
        past->obj = cur->obj;
    }

    next->obj = cur->obj;
    cur->obj = NONE;
}

int handleHero(struct tile *cur, int order, int pos)
{
    /* Try to move hero if still and received order */    

    if (objectStill(pos) && objectOrder(order))
    {
        /* Check if movement or pushing box is possible */
        if (canMove(cur, HERO, order))
        {
            /* OK, move */
            moveObject(cur, order);
        }
        else
        {
            /* Display blocking animation */
            blockObject(cur, order);
        }
    }
    return(0);
}

int handleBox(struct tile *cur, int order, int pos)
{
    /* Try to move box if still and received order */

    if (objectStill(pos) && objectOrder(order))
    {
        /* Check if box movement is possible */
        if (canMove(cur, BOX, order))
        {
            moveObject(cur, order);
        }
        else
        {
            /* Display blocking animation */
            blockObject(cur, order);
        }
    }
}

void setObjHandlers(handleObjType handle)
{
    handle[NONE] = NULL;
    handle[HERO] = handleHero;
    handle[BOX] = handleBox;
}
