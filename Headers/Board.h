
/*
** GameC engine
** Board
**
** Board game logic functions
*/

#ifndef BOARD_H
#define BOARD_H

#include <exec/types.h>

/* Dimension in tiles */

#define BOARD_TILE_WIDTH     20 
#define BOARD_TILE_HEIGHT    16

#define MAX_OBJECTS          10
#define MAX_BLOCKS           10

/* Basic kinds of tiles */

enum BasicType
{
    BASIC_FLOOR,
    BASIC_WALL,
    BASIC_BLOCK,
    BASIC_ITEM,
    BASIC_TYPES
};

/* Floor types */

enum FloorType
{
    FLOOR_NORMAL,
    FLOOR_FLAGSTONE,
    FLOOR_CRACKED,    
    FLOOR_HOLE,
    FLOOR_TYPES,
};

/* Crack sub types */

enum CrackType
{
    CRACK_LEVEL_1,
    CRACK_LEVEL_2,
    CRACK_LEVEL_3,
    CRACK_LEVEL_4,
    CRACK_TYPES
};

/* Object types (in object data) */

enum ObjectType
{
    OBJECT_HERO,
    OBJECT_BOX,
    OBJECT_SKULL,
    OBJECT_TYPES
};

/* Flagstone types (also box and gate) */

enum FlagSubType
{
    FLAG_CIRCLE,
    FLAG_SQUARE,
    FLAG_TRIANGLE,
    FLAG_TYPES
};

/* Block types */

enum BlockType
{
    BLOCK_DOOR,
    BLOCK_GATE,
    BLOCK_TYPES
};

/* Item types */

enum ItemType
{
    ITEM_CHERRY,
    ITEM_KEY,
    ITEM_TYPES
};

/* Key types (also door) */

enum KeyType
{
    KEY_BRONZE,
    KEY_SILVER,
    KEY_GOLD,
    KEY_TYPES
};

/* Identified block */
struct BlockData
{
    WORD x, y; /* Position */
};

struct ObjectData
{
    WORD type, sub; /* Object type */
    WORD ID;
    WORD x, y;
    WORD shift; /* 16 - 0, used for movement */
    WORD dx, dy; /* Direction */
    BOOL active;
};

/* Tile record */

struct Tile
{
    WORD basicType;
    WORD type; /* Primary type */
    WORD sub; /* Secondary type */
    WORD inID, outID; /* Object ID, not type */
};

/* Board record */

struct Board
{
    struct Tile tiles[ BOARD_TILE_HEIGHT ][ BOARD_TILE_WIDTH ];

    WORD allBoxes[FLAG_TYPES], placedBoxes[FLAG_TYPES];
    struct ObjectData objects[MAX_OBJECTS];
    struct BlockData blocks[MAX_BLOCKS];
    WORD blockCount;
    WORD objectCount;
};

/* Functions */

/* setObject:
 * 
 * Replace object on given tile.
 * 
 * board:   Pointer to board containing static data.
 * x:       X position
 * y:       Y position
 * type:    Object type
 * ID:      Object ID
 * 
 * Result:  none
 */

VOID setObject(struct Board *board, WORD x, WORD y, WORD type, WORD sub, WORD ID);
VOID remObject(struct Board *board, WORD ID);
VOID replaceObject(struct Board *board, WORD ID, WORD nx, WORD ny);

BOOL scanBoard(struct Board *board);

/* initBoard:
 * 
 * Reset board contents to initial.
 * 
 * board:   Pointer to board to be reset.
 * 
 * Result:  none
 */

VOID initBoard(struct Board *board);

/* setFloor:
 * 
 * Replace given tile to floor.
 * 
 * board:   Pointer to board containing static data.
 * tile:    Pointer to tile
 * type:    Floor type
 * sub:     Optional sub-type or 0
 * 
 * Result:  none
 */

VOID setFloor(struct Board *board, struct Tile *tile, WORD type, WORD sub);

/* setWall:
 * 
 * Replace given tile to wall.
 * 
 * board:   Pointer to board containing static data.
 * tile:    Pointer to tile
 * 
 * Result:  none
 */

VOID setWall(struct Board *board, struct Tile *tile);

/* setBlock:
 * 
 * Replace given tile to block.
 * 
 * board:   Pointer to board containing static data.
 * tile:    Pointer to tile
 * type:    Block type
 * sub:     Optional sub-type or 0
 * 
 * Result:  none
 */

VOID setBlock(struct Board *board, struct Tile *tile, WORD type, WORD sub);

/* setFloor:
 * 
 * Replace given tile to item
 * 
 * board:   Pointer to board containing static data.
 * tile:    Pointer to tile
 * type:    Item type
 * sub:     Optional sub-type or 0
 * 
 * Result:  none
 */

VOID setFloor(struct Board *board, struct Tile *tile, WORD type, WORD sub);

#endif /* BOARD_H */
