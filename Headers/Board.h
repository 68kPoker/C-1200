
/* Board logic - header file */

enum  Floors
{
    F_BACK,
    F_WALL,
    F_FLOOR,
    F_FLAGSTONE
};

enum Objects
{
    O_NONE,
    O_HERO,
    O_BOX
};

#define B_WIDTH  20
#define B_HEIGHT 16
#define B_SIZE   B_WIDTH * B_HEIGHT

/* Tree instance for board struct */

struct TreeBoard
{
    struct Board *board; /* Root board */
    struct Tile *tile; /* Tile */
    struct ObjectInfo *oi; /* Object info for this tile */
};

struct Board
{
    struct Tile *tiles;
    struct ObjectInfo *oi;
};

struct ObjectInfo
{

};

struct Tile
{
    WORD floor, object, objectID;
};

/* Functions */

/* Setup board */
void initBoard(struct TreeBoard *tb, struct Board *board);

void initTile(struct TreeBoard *tb, struct Tile *tile, struct ObjectInfo *oi);

void setObject(struct TreeBoard *tb);
