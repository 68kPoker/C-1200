
#include "GadgetData.h"
#include "Gadget.h"

/* Prepare plain boolean gadget with no imagery */
BOOL initGadget(struct gadgetData *gd, struct gadgetData *prev, WORD left, WORD top, WORD width, WORD height, WORD ID, VOID (*handle)(struct gadgetData *gd, struct IntuiMessage *msg))
{
    struct Gadget *gad;

    if (gd->gad = gad = AllocMem(sizeof(*gad), MEMF_PUBLIC|MEMF_CLEAR))
    {
        gad->NextGadget = NULL;
        if (prev)
        {
            prev->gad->NextGadget = gad;
        }
        gad->LeftEdge = left;
        gad->TopEdge = top;
        gad->Width = width;
        gad->Height = height;
        gad->Flags = GFLG_GADGHNONE;
        gad->Activation = GACT_IMMEDIATE;
        gad->GadgetType = GTYP_BOOLGADGET;
        gad->GadgetID = ID;        
        gad->UserData = (APTR)gd;

        gd->handleIDCMP = handle;
        return(TRUE);
    }
    return(FALSE);
}

VOID freeGadget(struct gadgetData *gd)
{
    FreeMem(gd->gad, sizeof(*gd->gad));
}
