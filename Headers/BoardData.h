
#include "TileData.h"

#define BOARD_TILE_WIDTH     (20) /* Dimension in tiles */
#define BOARD_TILE_HEIGHT    (16)

struct boardCoords
{
    unsigned int tileX, tileY;
};

struct boardData
{
    struct tileData tiles[ BOARD_TILE_HEIGHT ][ BOARD_TILE_WIDTH ];

    unsigned int allBoxes, placedBoxes;
    struct boardCoords heroCoords;
};
