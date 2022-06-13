
/* Functions that work on TileData */

long replaceFloor(int floorType, int floorID, struct floorData *fd, struct tileData *td, struct boardData *bd, struct gameData *gd, struct programData *pd);

long replaceObject(int objectType, int objectID, struct objectData *od, struct tileData *td, struct boardData *bd, struct gameData *gd, struct programData *pd);

long tile(struct tileData *td, struct boardData *bd, struct gameData *gd, struct programData *pd);
