
#include <exec/interrupts.h>
#include <graphics/gfx.h>

typedef struct CopperData
{
    struct ViewPort *vp;
    WORD sigBit;
    struct Task *task;
} COPPER;    

typedef struct ScreenData
{
    struct BitMap *gfx;
    PLANEPTR mask;
    struct BitMap *bm[2];
    struct TextFont *font;
    struct Screen *s;
    struct DBufInfo *dbi;
    struct MsgPort *sp;
    UBYTE safe, frame;
    COPPER cop;
    struct Interrupt is;
} SCREEN;

BOOL openScreen(SCREEN *s, STRPTR name);
void closeScreen(SCREEN *s);
