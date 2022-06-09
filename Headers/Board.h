
/*
** GameX engine
** Src > Board
*/

#ifndef BOARD_H
#define BOARD_H

#include <exec/types.h>

#include "Bobs.h"

#define B_WIDTH		20
#define B_HEIGHT	16

#define B_OBJECTS   10
#define B_FLOORS    10 /* Animated floors */

#define getOffset(x, y)       (y * B_WIDTH + x)
#define getTilePtr(b, offset) ((tile *)(b)->board + offset)

#define Left    -1
#define Right    1
#define Up      -B_WIDTH
#define Down     B_WIDTH

enum TileID
{
    TID_NONE,
    TID_HERO,
    TID_ACTIVE_BOX
};

/* If the floor member is T_NONE, there is no object on this tile */

enum Tiles
{
    T_NONE,
    T_BACK,
    T_WALL,
    T_FLOOR,
    T_BOX,
    T_FLAGSTONE,
    T_SAND,
    T_SANDBOX,
    T_MUD,
    T_HERO,
    T_COUNT
};

/* Animation frame counts */ 
#define GC_HERO   8
#define GC_MUD    4
#define GC_BUTTON 3 /* User interface parts count */
#define GC_TEXT   3

#define GC_DIR    2 /* Animation frames per direction */

enum /* Directions */
{
    DIR_LEFT,
    DIR_RIGHT,
    DIR_UP,
    DIR_DOWN
};

enum /* Gadget parts */
{
    GAD_LEFT,
    GAD_MIDDLE,
    GAD_RIGHT
};

typedef struct sTile
{
    short floor, floorID, type, typeID, flag;
} tile;

typedef struct sIdentifiedObject
{
    short offset;   
    short type; 
    short pos; /* 16-0 precise position */
    short dir; /* Direction offset */
    short trig; /* Movement trigger */
    short repeat; /* Repeat movement */
    short speed; /* Speed of movement */
    short active;
    struct bobData bob;
} identifiedObject;

typedef struct sIdentifiedFloor
{
    short offset;    
    short type;
} identifiedFloor;

typedef struct sBoard
{
    tile board[B_HEIGHT][B_WIDTH]; /* Array */
    short placed, boxes;
    identifiedFloor floorData[B_FLOORS];
    identifiedObject objectData[B_OBJECTS];
    struct BitMap *gfx;
} board;

extern tile tileTypes[T_COUNT];

extern LONG (*animateTile[T_COUNT])(struct sBoard *bp, struct sTile *tile);

/* Prototypes */

void constructTile(tile *op, short floor, short object);
void replaceObject(tile *op, short object, short objectID);
void removeObject(tile *op);
short moveObject(board *bp, tile *op, int offset);

void constructBoard(board *op);
short scanBoard(board *op);

/* Graphics frame table constructor */
VOID constructGfx(WORD *gfxCount, WORD maxCount);
VOID constructTypes(struct sTile *tileTypes, WORD maxCount);

#endif /* BOARD_H */