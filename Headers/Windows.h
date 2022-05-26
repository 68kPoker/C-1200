
/*
** GameX engine
** Src > Init > Windows
*/

#ifndef WINDOWS_H
#define WINDOWS_H

#include <exec/types.h>

struct windowData
{
    struct Window *w;
    struct gadgetData *activeGadget;
    BOOL done;
};

struct gadgetData 
{
    struct Gadget *gad;
    VOID (*handleIDCMP)(struct gadgetData *gd, struct IntuiMessage *msg);
};

BOOL initGadget(struct gadgetData *gd, struct gadgetData *prev, WORD left, WORD top, WORD width, WORD height, WORD ID, VOID (*handle)(struct gadgetData *gd, struct IntuiMessage *msg));
VOID freeGadget(struct gadgetData *gd);
BOOL openWindow(struct windowData *wd, struct screenData *sd, ULONG tag1, ...);
VOID closeWindow(struct windowData *wd);

#endif /* WINDOWS_H */
