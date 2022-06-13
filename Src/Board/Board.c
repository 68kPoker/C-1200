
#include <stdio.h>

#include "BoardData.h"
#include "Board.h"

/* clearBoard - Clear the board. */

long resetBoard(struct boardData *bd, struct gameData *gd, struct programData *pd)
{
    int x, y;

    for (y = 0; y < BOARD_TILE_HEIGHT; y++)
    {
        for (x = 0; x < BOARD_TILE_WIDTH; x++)
        {
            struct tileData *td = &bd->tiles[y][x];

            td->floor.type = FLOOR_TYPE_FLOOR;
            td->floor.ID = ID_NONE;

            td->object.ID = ID_NONE;

            if (x == 0 || y == 0 || x == BOARD_TILE_WIDTH - 1 || y == BOARD_TILE_HEIGHT - 1)
            {
                /* Border */
                td->object.type = OBJECT_TYPE_WALL;
            }
            else
            {
                td->object.type = OBJECT_TYPE_NONE;
            }
        }
    }

    bd->allBoxes = bd->placedBoxes = 0;
    bd->heroCoords.tileX = 1;
    bd->heroCoords.tileY = 1;

    return(0);
}

/* replaceHero - Change hero position */

long replaceHero(unsigned int tileX, unsigned int tileY, struct boardData *bd, struct gameData *gd, struct programData *pd)
{
    bd->heroCoords.tileX = tileX;
    bd->heroCoords.tileY = tileY;
    return(0);
}

/* scanBoard - Count boxes etc. */

long scanBoard(struct boardData *bd, struct gameData *gd, struct programData *pd)
{
    int x, y;

    bd->allBoxes = 0;

    for (y = 0; y < BOARD_TILE_HEIGHT; y++)
    {
        for (x = 0; x < BOARD_TILE_WIDTH; x++)
        {
            struct tileData *td = &bd->tiles[y][x];

            if (td->object.type == OBJECT_TYPE_BOX)
            {
                bd->allBoxes++;
            }
        }
    }
    return(0);
}

long drawBoard(FILE *f, struct boardData *bd, struct gameData *gd, struct programData *pd)
{
    int x, y;    

    for (y = 0; y < BOARD_TILE_HEIGHT; y++)
    {
        for (x = 0; x < BOARD_TILE_WIDTH; x++)
        {
            struct tileData *td = &bd->tiles[y][x];

            if (td->object.type != OBJECT_TYPE_NONE)
            {
                if (td->object.type == OBJECT_TYPE_WALL)
                {
                    fputc('#', f);
                }
                else if (td->object.type == OBJECT_TYPE_BOX)
                {
                    fputc('*', f);
                }
            }
            else
            {
                fputc('.', f);
            }
        }
        fputc('\n', f);
    }
    return(0);
}
