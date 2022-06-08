
#include <exec/memory.h>
#include <clib/exec_protos.h>

#include "Tile.h"

void init();
struct Tile *alloc();
void clear(struct Tile *t);

struct tileAPI tile = { init };

static void init()
{
    init.alloc = alloc;
    init.clear = clear;
}

static struct Tile *alloc()
{
    struct Tile *t;

    if (t = AllocVec(COLS * ROWS * sizeof(*t)))
    {
        tile.clear(t);
        return(t);
    }
    return(NULL);
}

static void clear(struct Tile *t)
{
    WORD i, j;
    struct Tile floor = { { FLOOR }, { NONE } }, wall = { { WALL }, { NONE } };

    for (i = 0; i < ROWS; i++)
    {
        for (j = 0; j < COLS; j++)
        {
            if (i == 0 || i == ROWS - 1 || j == 0 || j == COLS - 1)
            {
                *t++ = wall;
            }
            else
            {
                *t++ = floor;
            }
        }
    }
}