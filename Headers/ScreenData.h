
#include <exec/interrupts.h>
#include <devices/inputevent.h>
#include <graphics/gfx.h>

#include "WindowData.h"

struct copperData
{
    struct ViewPort *vp;
    WORD sigBit;
    struct Task *task;
};

/* Root record */
struct screenData
{
    struct BitMap *bm[2];
    struct TextFont *font;
    struct Screen *s;
    struct DBufInfo *dbi;
    struct MsgPort *safePort;
    BOOL safe;
    WORD frame;
    struct Interrupt is;
    struct copperData cd;

    struct windowData wd; /* Main window */
    struct BitMap *gfx;
    PLANEPTR mask;

    struct IOStdReq *joyIO;
    struct InputEvent joyIE;
};
