
#include "TileData.h"
#include "Tile.h"

long replaceFloor( int floorType, int floorID, struct floorData *fd, struct tileData *td, struct boardData *bd, struct gameData *gd, struct programData *pd )
{
    fd->type = floorType;
    fd->ID = floorID;
    return(0);
}

long replaceObject( int objectType, int objectID, struct objectData *od, struct tileData *td, struct boardData *bd, struct gameData *gd, struct programData *pd )
{
    od->type = objectType;
    od->ID = objectID;
    return(0);
}

long tile( struct tileData *td, struct boardData *bd, struct gameData *gd, struct programData *pd )
{
    return(0);
}
