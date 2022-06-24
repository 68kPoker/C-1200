
#include <graphics/gfx.h>

#define RowBytes(w) ((((w)+15)>>4)<<1)

enum
{
    HORIZ,
    VERT
};    

typedef struct Tile
{
    struct BitMap *gfx;
    UBYTE gfxX, gfxY;
} TILE;

BOOL chkTile(struct RastPort *rp, UBYTE dx, UBYTE dy);
void bltTile(TILE *gfx, struct RastPort *rp, UBYTE dx, UBYTE dy);
void bltAddTile(TILE *floor, TILE *object, BYTE offset, UBYTE orient, PLANEPTR mask, struct RastPort *rp, UBYTE dx, UBYTE dy);
