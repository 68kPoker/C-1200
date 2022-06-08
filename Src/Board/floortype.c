
/* 
** Floor_types.c
*/

#include "floortype.h"
#include "objtype.h"

#define setCounter(var, val) var = val
#define floorLeft(type) left == type
#define floorVisited(type) visited == type

/*
** Graphics frames 
*/

int setCount(int *array, int type, int counter, int count)
{
    array[type] = counter;

    return(counter + count);
}

void calcFloorTypeGfx(int *gfx)
{
    int counter = 0;

    counter = setCount(gfx, BACK, counter, 1);
    counter = setCount(gfx, WALL, counter, 1);
    counter = setCount(gfx, FLOOR, counter, 1);
    counter = setCount(gfx, FLAGSTONE, counter, 1);
}

int handleFlagstone(int left, int visited)
{
    /* Handle box placement + counter here */
    /* If this floor has been visited by a box, increment counter */
    /* If this floor has been left by a box, decrement counter */

    if (floorLeft(BOX))
    {
        setCounter(placed, placed - 1);
    }
    if (floorVisited(BOX))
    {
        setCounter(placed, placed + 1);
    }
    return(0);
}

void setFloorHandlers(handleFloorType handle)
{
    handle[BACK] = NULL;
    handle[WALL] = NULL;
    handle[FLOOR] = NULL;
    handle[FLAGSTONE] = handleFlagstone;
}
