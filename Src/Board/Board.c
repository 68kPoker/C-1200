
/* Board logic */

#include "Board.h"

void initTile(struct TreeBoard *tb, struct Tile *tile)
{
    tile->floor = F_FLOOR;
    tile->object = O_NONE;
    tile->objectID = 0;
}

void initBoard(struct TreeBoard *tb, struct Board *board)
{
    WORD x, y;
    struct Tile *tile = board->tiles;

    for (y = 0; y < B_HEIGHT; y++)
    {
        for (x = 0; x < B_WIDTH; x++)
        {            
            tb->tile = tile;
            initTile(tb, tile++);
        }
    }
}
