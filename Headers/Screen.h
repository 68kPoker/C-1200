
/*
** GameC engine
** Screen
*/

#ifndef SCREEN_H
#define SCREEN_H

#include <exec/types.h>
#include <exec/interrupts.h>
#include <devices/inputevent.h>
#include <graphics/gfx.h>

#include "Window.h"

#define SCREEN_BITMAP_WIDTH  320
#define SCREEN_BITMAP_HEIGHT 256

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

BOOL openScreen(struct screenData *sd, STRPTR title, ULONG modeID, struct Rectangle *dclip, ULONG *pal, struct TextAttr *ta);
VOID closeScreen(struct screenData *sd);
BOOL addCop(struct screenData *sd, STRPTR name, BYTE pri, WORD coplen, VOID (*addCustomCop)(struct screenData *sd, struct UCopList *ucl));
VOID remCop(struct screenData *sd);
BOOL addDBuf(struct screenData *sd);
VOID remDBuf(struct screenData *sd);
VOID safeToDraw(struct screenData *sd);
VOID swapBuf(struct screenData *sd);

#endif /* SCREEN_H */
