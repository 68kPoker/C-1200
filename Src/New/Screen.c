
/* General Screen functions */

#include <intuition/screens.h>
#include <hardware/custom.h>
#include <hardware/intbits.h>
#include <graphics/gfxmacros.h>
#include <exec/memory.h>

#include <clib/exec_protos.h>
#include <clib/diskfont_protos.h>
#include <clib/intuition_protos.h>
#include <clib/graphics_protos.h>

#include "Screen.h"
#include "IFF.h"

__far extern struct Custom custom;

struct Rectangle dclip =
{
    0, 0, 319, 255
};

struct TextAttr ta =
{
    "centurion.font", 9, FS_NORMAL, FPF_DISKFONT|FPF_DESIGNED
};    

__saveds __asm LONG myCopper(register __a1 COPPER *cop)
{
    if (!(cop->vp->Modes & VP_HIDE))
    {
        Signal(cop->task, 1L << cop->sigBit);
    }
    return(0);    
}

BOOL openScreen(SCREEN *s, STRPTR name)
{
    struct ColorMap *cm = NULL;
    ULONG srcID;

    if (s->gfx = loadPicture(name, &cm, &s->mask, &srcID))
    {
        if (s->bm[0] = AllocBitMap(320, 256, s->gfx->Depth, BMF_INTERLEAVED|BMF_CLEAR, NULL))
        {
            if (s->bm[1] = AllocBitMap(320, 256, s->gfx->Depth, BMF_INTERLEAVED|BMF_CLEAR, NULL))
            {
                if (s->font = OpenDiskFont(&ta))
                {
                    struct Screen *pubs;
                    
                    if (pubs = LockPubScreen(NULL))
                    {
                        ULONG modeID;
                        
                        modeID = BestModeID(
                            BIDTAG_MonitorID,   GetVPModeID(&pubs->ViewPort) & MONITOR_ID_MASK,
                            BIDTAG_SourceID,    srcID,
                            BIDTAG_Depth,       s->gfx->Depth,
                            BIDTAG_NominalWidth,    320,
                            BIDTAG_NominalHeight,   256,
                            TAG_DONE);    
                            
                        UnlockPubScreen(NULL, pubs);
                        
                        if (modeID != INVALID_ID)
                        {
                            if (s->s = OpenScreenTags(NULL,
                                SA_DisplayID,   modeID,
                                SA_DClip,       &dclip,
                                SA_Font,        &ta,
                                SA_BitMap,      s->bm[0],
                                SA_Quiet,       TRUE,
                                SA_Exclusive,   TRUE,
                                SA_BackFill,    LAYERS_NOBACKFILL,
                                SA_ShowTitle,   FALSE,    
                                TAG_DONE))
                            {
                                struct ViewPort *vp = &s->s->ViewPort;
                                struct ColorMap *dcm = vp->ColorMap;
                                ULONG rgb[3];
                                WORD i;
                                
                                for (i = 0; i < dcm->Count; i++)
                                {
                                    GetRGB32(cm, i, 1, rgb);
                                    SetRGB32CM(dcm, i, rgb[0], rgb[1], rgb[2]);
                                }
                                MakeScreen(s->s);
                                RethinkDisplay();
                                
                                if (s->dbi = AllocDBufInfo(vp))
                                {
                                    if (s->sp = CreateMsgPort())
                                    {
                                        COPPER *cop = &s->cop;
                                        struct Interrupt *is = &s->is;
                                        
                                        s->dbi->dbi_SafeMessage.mn_ReplyPort = s->sp;
                                    
                                        s->safe = TRUE;
                                        s->frame = 1;
                                    
                                        cop->vp = vp;
                                        cop->task = FindTask(NULL);
                                        if ((cop->sigBit = AllocSignal(-1)) != -1)
                                        {
                                            struct UCopList *ucl;
                                            
                                            is->is_Code = (void(*)())myCopper;
                                            is->is_Data = (APTR)cop;
                                            is->is_Node.ln_Pri = 0;
                                        
                                            if (ucl = AllocMem(sizeof(*ucl), MEMF_PUBLIC|MEMF_CLEAR))
                                            {
                                                CINIT(ucl, 3);
                                                CWAIT(ucl, 0, 0);
                                                CMOVE(ucl, custom.intreq, INTF_SETCLR|INTF_COPER);
                                                CEND(ucl);
                                                
                                                Forbid();
                                                vp->UCopIns = ucl;
                                                Permit();
                                            
                                                RethinkDisplay();
                                                AddIntServer(INTB_COPER, is);
                                            
                                                FreeColorMap(cm);
                                                return(TRUE);
                                            }    
                                            FreeSignal(cop->sigBit);
                                        }    
                                        DeleteMsgPort(s->sp);
                                    }    
                                    FreeDBufInfo(s->dbi);
                                }    
                                CloseScreen(s->s);
                            }    
                        }   
                    }     
                    CloseFont(s->font);
                }    
                FreeBitMap(s->bm[1]);
            }
            FreeBitMap(s->bm[0]);
        }
        FreeBitMap(s->gfx);
        FreeVec(s->mask);
        FreeColorMap(cm);
    }
    return(FALSE);
}

void closeScreen(SCREEN *s)
{
    RemIntServer(INTB_COPER, &s->is);
    FreeSignal(s->cop.sigBit);    
    
    if (!s->safe)
    {
        while (!GetMsg(s->sp))
        {
            WaitPort(s->sp);
        }    
    }    
    
    DeleteMsgPort(s->sp);
    FreeDBufInfo(s->dbi);
    CloseScreen(s->s);
    CloseFont(s->font);
    FreeBitMap(s->bm[1]);
    FreeBitMap(s->bm[0]);
    FreeBitMap(s->gfx);
    FreeVec(s->mask);
}
