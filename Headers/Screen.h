
/*
** GameX engine
** Src > Init > Screen
*/

#ifndef SCREEN_H
#define SCREEN_H

#include <exec/interrupts.h>

struct copperData
{
    struct ViewPort *vp;
    WORD sigBit;
    struct Task *task;
};

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
};

BOOL openScreen(struct screenData *sd, STRPTR title, ULONG modeID, struct Rectangle *dclip, struct BitMap *bm[], ULONG *pal, struct TextAttr *ta);
VOID closeScreen(struct screenData *sd);
BOOL addCop(struct screenData *sd, STRPTR name, BYTE pri, WORD coplen, VOID (*addCustomCop)(struct screenData *sd, struct UCopList *ucl));
VOID remCop(struct screenData *sd);
BOOL addDBuf(struct screenData *sd);
VOID remDBuf(struct screenData *sd);
VOID safeToDraw(struct screenData *sd);
VOID swapBuf(struct screenData *sd);

#endif /* SCREEN_H */
