
#include <exec/interrupts.h>
#include <devices/inputevent.h>
#include <graphics/gfx.h>

#include "Bobs.h"

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

    struct windowData *wd; /* Pointer to main window */
    struct BitMap *gfx;

    struct IOStdReq *joyIO;
    struct InputEvent joyIE;

    struct List bobs; /* Bobs list */
    
    PLANEPTR mask;
};
