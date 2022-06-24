
#include <exec/types.h>

enum
{
    CTRL_C,
    SAFE, /* Safe to draw */
    COPPER, /* Copper at line 0 */
    IDCMP,
    EVENTS
};    

enum
{
    T_BACK,
    T_WALL,
    T_FLOOR,
    T_BOX,
    T_FLAGSTONE,
    T_SAND,
    T_STONE,
    T_SAND_BAG,
    T_MUD,
    T_WATER
};    

struct loop
{
    struct event
    {
        ULONG mask;
        LONG (*func)(APTR obj);
        APTR obj;
    } events[ EVENTS ];
    ULONG total;
};

struct board
{
    WORD tiles[16][20];
};    

struct screen
{
    struct BitMap *bm[2];
    VOID (*prepBitMap)(APTR obj, struct BitMap *bm);
    APTR obj;
    struct Screen *s;
    struct DBufInfo *dbi;
    struct MsgPort *sp;
    BOOL safe;
    UWORD frame;
    struct Interrupt is;
    struct copper
    {
        struct ViewPort *vp;
        WORD sigbit;
        struct Task *task;
    } cop;    
    WORD counter;
    WORD tile, object;
    WORD posx, posy, prevPosx[2], prevPosy[2];
    struct window *w;
    struct BitMap *gfx;
    PLANEPTR mask;
    WORD trigx, trigy, dirx, diry, delay;
    BOOL bg[2];
    struct board board;
    struct loop *l;
    WORD heroX, heroY;
    WORD cursX, cursY;
    BOOL paint;
};    

struct window
{
    struct Window *w;
};
