
/*
** GameX engine
** Src > Init > Windows
*/

#include <intuition/intuition.h>
#include <exec/memory.h>

#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>

#include "Windows.h"

BOOL initGadget(struct gadgetData *gd, struct gadgetData *prev, WORD ID)
{
    struct Gadget *gad;

    if (gd->gad = gad = AllocMem(sizeof(*gad), MEMF_PUBLIC|MEMF_CLEAR))
    {
        gad->NextGadget = NULL;
        if (prev)
        {
            prev->gad->NextGadget = gad;
        }
        gad->Flags = GFLG_GADGHNONE;
        gad->Activation = GACT_IMMEDIATE;
        gad->GadgetType = GTYP_BOOLGADGET;
        gad->GadgetID = ID;
        gad->UserData == (APTR)gd;
        return(TRUE);
    }
    return(FALSE);
}

VOID freeGadget(struct gadgetData *gd)
{
    FreeMem(gd->gad, sizeof(*gd->gad));
}

BOOL openWindow(struct windowData *wd, struct screenData *sd, ULONG tag1, ...)
{
    struct Window *w;

    if (wd->w = w = OpenWindowTags(NULL,
        WA_CustomScreen,    sd->s,
        WA_Borderless,      TRUE,
        WA_RMBTrap,         TRUE,
        WA_SimpleRefresh,   TRUE,
        WA_BackFill,        LAYERS_NOBACKFILL,
        WA_ReportMouse,     TRUE,
        TAG_MORE,           &tag1))
    {
        w->UserData = (APTR)wd;

        return(TRUE);
    }
    return(FALSE);
}

VOID closeWindow(struct windowData *wd)
{
    CloseWindow(wd->w);
}
