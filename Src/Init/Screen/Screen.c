
/*
** GameX engine
** Src > Init > Screen
*/

#include <intuition/screens.h>
#include <exec/memory.h>
#include <exec/interrupts.h>
#include <graphics/gfxmacros.h>
#include <hardware/custom.h>
#include <hardware/intbits.h>

#include <clib/exec_protos.h>
#include <clib/diskfont_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>

#include "Screen.h"

__far extern struct Custom custom;

__saveds __asm ULONG myCop(register __a1 struct copperData *cd)
{
    if (!(cd->vp->Modes & VP_HIDE))
    {
        Signal(cd->task, 1L << cd->sigBit);
    }
    return(0);
}

BOOL openScreen(struct screenData *sd, STRPTR title, ULONG modeID, struct Rectangle *dclip, struct BitMap *bm[], ULONG *pal, struct TextAttr *ta)
{
    if (sd->font = OpenDiskFont(ta))
    {
        if (sd->s = OpenScreenTags(NULL,
            SA_DisplayID,   modeID,
            SA_DClip,       dclip,
            SA_BitMap,      bm[0],
            SA_Colors32,    pal,
            SA_Title,       title,
            SA_Font,        ta,
            SA_ShowTitle,   FALSE,
            SA_Quiet,       TRUE,
            SA_Exclusive,   TRUE,
            SA_BackFill,    LAYERS_NOBACKFILL,
            TAG_DONE
        {
            sd->bm[0] = bm[0];
            sd->bm[1] = bm[1];
            
            sd->s->UserData = (APTR)sd;
            return(TRUE);
        }
        CloseFont(sd->font);
    }
    return(FALSE);
}

VOID closeScreen(struct screenData *sd)
{
    CloseScreen(sd->s);
    CloseFont(sd->font);
}

BOOL addCop(struct screenData *sd, STRPTR name, BYTE pri, WORD coplen, VOID (*addCustomCop)(struct screenData *sd, struct UCopList *ucl))
{
    struct Interrupt *is = &sd->is;
    struct copperData *cd = &sd->cd;

    is->is_Code = myCop;
    is->is_Data = (APTR)cd;
    is->is_Node.ln_Name = name;
    is->is_Node.ln_Pri = pri;

    if ((cd->sigBit = AllocSignal(-1)) != -1)
    {
        struct UCopList *ucl;

        cd->vp = &sd->s->ViewPort;
        cd->task = FindTask(NULL);

        if (ucl = AllocMem(sizeof(*ucl), MEMF_PUBLIC|MEMF_CLEAR))
        {
            CINIT(ucl, coplen + 3);
            CWAIT(ucl, 0, 0);
            CMOVE(ucl, custom.intreq, INTF_SETCLR|INTF_COPER);            
            if (addCustomCop)
            {            
                addCustomCop(sd, ucl);
            }
            CEND(ucl);

            Forbid();
            cd->vp->UCopIns = ucl;
            Permit();

            RethinkDisplay();

            AddIntServer(INTB_COPER, is);
            return(TRUE);
        }
        FreeSignal(cd->sigBit);
    }
    return(FALSE);
}

VOID remCop(struct screenData *sd)
{
    RemIntServer(INTB_COPER, &sd->is);
    FreeSignal(sd->cd.sigBit);
}

BOOL addDBuf(struct screenData *sd)
{
    if (sd->dbi = AllocDBufInfo(&sd->s->ViewPort))
    {
        if (sd->safePort = CreateMsgPort())
        {
            sd->dbi->dbi_SafeMessage.mn_ReplyPort = sd->safePort;
            sd->safe = TRUE;
            sd->frame = 1;

            return(TRUE);
        }
        FreeDBufInfo(sd->dbi);
    }
    return(FALSE);
}

/* Ensure it's safe to draw */
VOID safeToDraw(struct screenData *sd)
{
    if (!sd->safe)
    {
        while (!GetMsg(sd->safePort))
        {
            WaitPort(sd->safePort);
        }
        sd->safe = TRUE;
    }
}

/* Display buffer */
VOID swapBuf(struct screenData *sd)
{
    if (sd->safe)
    {
        WORD frame = sd->frame;

        ChangeVPBitMap(&sd->s->ViewPort, sd->bm[frame], sd->dbi);
        sd->frame = frame ^ 1;
        sd->safe = FALSE:
    }
}

VOID remDBuf(struct screenData *sd)
{
    if (!sd->safe)
    {
        while (!GetMsg(sd->safePort))
        {
            WaitPort(sd->safePort);
        }
    }
    DeleteMsgPort(sd->safePort);
    FreeDBufInfo(sd->dbi);
}
