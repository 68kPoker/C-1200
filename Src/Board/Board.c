
/*
** GameC engine
** Board
**
** Board game logic functions
*/

#include "Board.h"

VOID setObject(struct Board *board, WORD x, WORD y, WORD type, WORD sub, WORD ID)
{
    struct ObjectData *od = board->objects + ID - 1;
    
    od->active = TRUE;
    od->dx = 1; /* Direction */
    od->dy = 0;
    od->x = x; /* Position */
    od->y = y;
    od->shift = 0;
    od->type = type;
    od->sub = sub;

    board->tiles[y][x].inID = ID;

    board->objectCount++;
}

VOID remObject(struct Board *board, WORD ID)
{
    struct ObjectData *od = board->objects + ID - 1;

    od->active = FALSE;

    board->tiles[od->y][od->x].inID = 0;

    if (ID == board->objectCount)
    {
        /* It is possible to decrement counter by 1 or more inactive objects */
        while (board->objectCount > 0 && !board->objects[board->objectCount - 1].active)
        {
            board->objectCount--;
        }
    }
}

/* initBoard:
 * 
 * Reset board contents to initial.
 * 
 * board:   Pointer to board to be reset.
 * 
 * Result:  none
 */

VOID initBoard(struct Board *board)
{
    WORD x, y;
    WORD i;

    for (y = 0; y < BOARD_TILE_HEIGHT; y++)
    {
        for (x = 0; x < BOARD_TILE_WIDTH; x++)
        {
            struct Tile *tile = &board->tiles[y][x];

            tile->inID = tile->outID = 0; /* Valid state */

            if (x == 0 || y == 0 || x == BOARD_TILE_WIDTH - 1 || y == BOARD_TILE_HEIGHT - 1)
            {
                /* Border */
                setWall(board, tile);
            }
            else
            {
                setFloor(board, tile, FLOOR_NORMAL, 0);
            }
        }
    }

    for (i = 0; i < FLAG_TYPES; i++)
    {
        board->allBoxes[i] = board->placedBoxes[i] = 0;            
    }

    board->objectCount = 0;
    board->blockCount = 0;

    /* Add hero object */
    setObject(board, 1, 1, OBJECT_HERO, 0, 1);
}

/* replaceObject - Change object position */

VOID replaceObject(struct Board *board, WORD ID, WORD nx, WORD ny)
{
    struct ObjectData *od = board->objects + ID - 1;

    board->tiles[od->y][od->x].inID = 0;

    od->x = nx;
    od->y = ny;

    board->tiles[ny][nx].inID = ID;
    
    return(0);
}

/* scanBoard - Count boxes etc. */

BOOL scanBoard(struct Board *board)
{
    WORD x, y, i;

    for (i = 0; i < FLAG_TYPES; i++)
    {
        board->allBoxes[i] = board->placedBoxes[i] = 0;            
    }

    for (i = 0; i < board->objectCount; i++)
    {
        struct ObjectData *od = board->objects + i;

        if (od->active)
        {
            if (od->type == OBJECT_BOX)
            {
                board->allBoxes[od->sub]++;
            }
        }
    }
    return(TRUE);
}

VOID setFloor(struct Board *board, struct Tile *tile, WORD type, WORD sub)
{
    tile->basicType = BASIC_FLOOR;
    tile->type = type;
    tile->sub = sub;
}

VOID setWall(struct Board *board, struct Tile *tile)
{
    tile->basicType = BASIC_WALL;
}

VOID setBlock(struct Board *board, struct Tile *tile, WORD type, WORD sub)
{
    WORD i, x = (WORD)(tile - board->tiles) % BOARD_TILE_WIDTH, y = (WORD)(tile - board->tiles) / BOARD_TILE_WIDTH;

    tile->basicType = BASIC_BLOCK;

    tile->type = type;
    tile->sub = sub;

    for (i = 0; i < board->blockCount; i++)
    {
        if (board->blocks[i].x == x && board->blocks[i].y == y)
        {
            break;
        }
    }

    if (i == board->blockCount)
    {
        board->blockCount++;
        board->blocks[i].x = x;
        board->blocks[i].y = y;
    }
}
