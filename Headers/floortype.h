
/* 
** Floor_types.h
*/

#include <exec/types.h>

enum
{
    BACK, WALL, FLOOR, FLAGSTONE, FLOORS
};

/*
** Graphics frames 
*/

/* Calc distribution */
extern void calcFloorTypeGfx(int *gfx);

/* Calc sum */
extern int setCount(int *array, int type, int counter, int count);

/*
** Logic 
*/

/* Array of handlers */
typedef int (*handleFloorType[FLOORS])(int left, int visited);

extern void setFloorHandlers(handleFloorType handle);
