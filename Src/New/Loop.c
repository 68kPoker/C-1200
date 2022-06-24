
#include <stdio.h>
#include <dos/dos.h>

#include <intuition/intuition.h>
#include <graphics/gfxmacros.h>
#include <graphics/rpattr.h>
#include <hardware/custom.h>
#include <hardware/intbits.h>
#include <exec/interrupts.h>
#include <exec/memory.h>

#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#include <clib/graphics_protos.h>
#include <clib/diskfont_protos.h>

#include "IFF.h"
#include "Loop.h"
#include "Blitter.h"

__far extern struct Custom custom;

struct TextAttr ta =
{
    "centurion.font", 9, FS_NORMAL, FPF_DISKFONT|FPF_DESIGNED
};    

__saveds __asm LONG myCopper(register __a1 struct copper *cop)
{
    if (!(cop->vp->Modes & VP_HIDE))
    {
        Signal(cop->task, 1L << cop->sigbit);
    }
    return(0);    
}

VOID prepBitMap(struct gfx *gfx, struct BitMap *bm)
{
    struct RastPort rp;
    
    InitRastPort(&rp);
    rp.BitMap = bm;
    
    SetRPAttrs(&rp,
        RPTAG_APen, 0,
        RPTAG_OutlinePen, 1,
        TAG_DONE);
    
    RectFill(&rp, 0, 0, GetBitMapAttr(bm, BMA_WIDTH) - 1, GetBitMapAttr(bm, BMA_HEIGHT) - 1);
}

BOOL openScreen(struct screen *s, UBYTE depth)
{
    struct Rectangle dclip = { 0, 0, 319, 255 };
    
    if (s->bm[0] = AllocBitMap(320, 256, depth, BMF_DISPLAYABLE|BMF_INTERLEAVED, NULL))
    {
        s->prepBitMap(s->obj, s->bm[0]);
    
        if (s->bm[1] = AllocBitMap(320, 256, depth, BMF_DISPLAYABLE|BMF_INTERLEAVED, NULL))
        {
            struct Screen *pubs;
            
            s->prepBitMap(s->obj, s->bm[1]);
            
            if (pubs = LockPubScreen(NULL))
            {
                ULONG modeID = BestModeID(
                    BIDTAG_MonitorID,       GetVPModeID(&pubs->ViewPort) & MONITOR_ID_MASK,
                    BIDTAG_NominalWidth,    320,
                    BIDTAG_NominalHeight,   256,
                    BIDTAG_Depth,           depth,
                    TAG_DONE);
                    
                UnlockPubScreen(NULL, pubs);
                
                WaitBlit();        
                
                if (s->s = OpenScreenTags(NULL,
                    SA_BitMap,      s->bm[0],
                    SA_Font,        &ta,
                    SA_DClip,       &dclip,
                    SA_DisplayID,   modeID,
                    SA_Quiet,       TRUE,
                    SA_BackFill,    LAYERS_NOBACKFILL,
                    SA_ShowTitle,   FALSE,
                    SA_Exclusive,   TRUE,
                    SA_SharePens,   TRUE,
                    TAG_DONE))
                {
                    struct ViewPort *vp = &s->s->ViewPort;
                    
                    if (s->dbi = AllocDBufInfo(vp))
                    {
                        if (s->sp = CreateMsgPort())
                        {
                            s->safe = TRUE;
                            s->frame = 1;
                            s->dbi->dbi_SafeMessage.mn_ReplyPort = s->sp;
                            s->is.is_Data = (APTR)&s->cop;
                            s->is.is_Code = (APTR)myCopper;
                            s->is.is_Node.ln_Pri = 0;
                            s->cop.vp = vp;
                            s->cop.task = FindTask(NULL);
                            if ((s->cop.sigbit = AllocSignal(-1)) != -1)
                            {
                                struct UCopList *ucl;
                                
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
                                    
                                    AddIntServer(INTB_COPER, &s->is);
                                    
                                    s->s->UserData = (APTR)s;
                                    
                                    return(TRUE);
                                }    
                                FreeSignal(s->cop.sigbit);
                            }
                            DeleteMsgPort(s->sp);
                        }        
                        FreeDBufInfo(s->dbi);
                    }    
                    CloseScreen(s->s);
                }    
            }    
            FreeBitMap(s->bm[1]);
        }
        FreeBitMap(s->bm[0]);    
    }
    return(FALSE);    
}

VOID closeScreen(struct screen *s)
{
    RemIntServer(INTB_COPER, &s->is);
    FreeSignal(s->cop.sigbit);
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
    FreeBitMap(s->bm[1]);
    FreeBitMap(s->bm[0]);
}

BOOL openWindow(struct window *w, struct screen *s)
{
    if (w->w = OpenWindowTags(NULL,
        WA_CustomScreen,    s->s,
        WA_Left,            0,
        WA_Top,             0,
        WA_Width,           s->s->Width,
        WA_Height,          s->s->Height,
        WA_Borderless,      TRUE,
        WA_SimpleRefresh,   TRUE,
        WA_BackFill,        LAYERS_NOBACKFILL,
        WA_RMBTrap,         TRUE,
        WA_Activate,        TRUE,    
        WA_IDCMP,           IDCMP_RAWKEY|IDCMP_MOUSEBUTTONS|IDCMP_MOUSEMOVE,
        WA_ReportMouse,     TRUE,
        TAG_DONE))
    {
        s->w = w;
        w->w->UserData = (APTR)w;
        return(TRUE);
    }
    return(FALSE);    
}

VOID closeWindow(struct window *w)
{
    CloseWindow(w->w);
}    

LONG loop(struct loop *l)
{
    BOOL done = FALSE;
    
    while (!done)
    {
        WORD i;
        ULONG result, total = l->total;
        result = Wait(total);
        for (i = 0; i < EVENTS; i++)
        {
            struct event *e = l->events + i;
            if (result & e->mask)
            {
                if (done = e->func(e->obj))
                {
                    break;
                }    
            }
        }    
    }            
    return(TRUE);
}

LONG ctrlC(APTR obj)
{
    printf("CTRL-C\n");
    return(TRUE);
}

LONG safe(struct screen *s)
{
    struct RastPort *rp = s->w->w->RPort;
    UBYTE text[5];
    WORD anim = 10;
    
    rp->BitMap = s->bm[s->frame];
    
    if (!s->safe)
    {
        while (!GetMsg(s->sp))
        {
            WaitPort(s->sp);
        }
        s->safe = TRUE;    
    }    
    /* Draw here */
    
    if (!s->bg[s->frame])
    {
        WORD x, y;
        
        bltBitMap(s->gfx, 0, 208, rp->BitMap, 0, 0, 320, 16, 0xc0);
        bltBitMap(s->gfx, 0, 224, rp->BitMap, 0, 224, 320, 32, 0xc0);
        
        for (y = 1; y < 14; y++)
        {
            for (x = 0; x < 20; x++)
            {
                WORD sx = 2;
                if (x == 0 || x == 19 || y == 1 || y == 13)
                {
                    sx = 1;
                }
                else if (x == 1 || x == 18 || y == 2 || y == 12)
                {    
                    sx = 2;
                }    
                bltBitMap(s->gfx, sx << 4, 0 << 4, rp->BitMap, x << 4, y << 4, 16, 16, 0xc0);
            }    
        }    
        s->bg[s->frame] = TRUE;
    }
    
    if (s->prevPosx[s->frame] != s->posx || s->prevPosy[s->frame] != s->posy)
    {
        bltBitMapRastPort(s->gfx, 32, 0, rp, s->prevPosx[s->frame] & 0xfff0, s->prevPosy[s->frame] & 0xfff0, 16, 16, 0xc0);    
        bltBitMapRastPort(s->gfx, 32, 0, rp, (s->prevPosx[s->frame] + 15) & 0xfff0, (s->prevPosy[s->frame] + 15) & 0xfff0, 16, 16, 0xc0);            
    }      
    
    if (s->delay == 0)
    {
        if ((s->trigx != 0 || s->trigy != 0) && (s->trigx == 0 || s->trigy == 0))
        {
            WORD x = s->heroX, y = s->heroY;
            
            WORD tile = s->board.tiles[y + s->trigy][x + s->trigx];
            
            if (tile == T_FLOOR)
            {
                s->dirx = s->trigx * 2;
                s->diry = s->trigy * 2;        
                s->delay = 16;
                s->heroX += s->trigx;
                s->heroY += s->trigy;
            }    
            else
            {
                s->dirx = s->trigx;
                s->diry = s->trigy;
            }    
        }    
    }
    
    if (s->delay > 0)
    {
        s->delay -= 2;
        s->posx += s->dirx;
        s->posy += s->diry;
    }        
    
    if (s->dirx > 0)
    {
        anim = 12;
    }
    else if (s->diry > 0)
    {
        anim = 14;
    }    
    else if (s->diry < 0)
    {
        anim = 16;
    }
    
    anim += (s->delay >> 3) & 0x1;
    
    bltBitMapRastPort(s->gfx, 32, 0, rp, s->posx & 0xfff0, s->posy & 0xfff0, 16, 16, 0xc0);
    
    bltMaskBitMapRastPort(s->gfx, anim << 4, 1 << 4, rp, s->posx, s->posy, 16, 16, 0xc0, s->mask);
    
    s->prevPosx[s->frame] = s->posx;
    s->prevPosy[s->frame] = s->posy;    
    
    return(FALSE);
}

LONG copper(struct screen *s)
{   
    if (s->safe)
    {
        WaitBlit();
        ChangeVPBitMap(s->cop.vp, s->bm[s->frame], s->dbi);
        s->frame ^= 1;
        s->safe = FALSE;
    }    
    
    return(FALSE);
}

LONG idcmp(struct window *w)
{
    struct IntuiMessage *msg;
    struct Screen *scr = w->w->WScreen;
    struct screen *s = (struct screen *)scr->UserData;
 
    while (msg = (struct IntuiMessage *)GetMsg(w->w->UserPort))
    {
        ULONG class = msg->Class;
        UWORD code = msg->Code;
        WORD mx = msg->MouseX;
        WORD my = msg->MouseY;
        APTR iaddr = msg->IAddress;
    
        ReplyMsg((struct Message *)msg);
        
        if (class == IDCMP_MOUSEBUTTONS)
        {
            if (code == IECODE_LBUTTON)
            {
                s->board.tiles[my >> 4][mx >> 4] = s->tile;
                
                w->w->RPort->BitMap = s->bm[0];
                
                bltBitMapRastPort(s->gfx, s->tile << 4, 0 << 4, w->w->RPort, mx & 0xfff0, my & 0xfff0, 16, 16, 0xc0);
                
                w->w->RPort->BitMap = s->bm[1];
                
                bltBitMapRastPort(s->gfx, s->tile << 4, 0 << 4, w->w->RPort, mx & 0xfff0, my & 0xfff0, 16, 16, 0xc0);
                s->paint = TRUE;
                s->cursX = mx >> 4;
                s->cursY = my >> 4;
            }
            else if (code == (IECODE_LBUTTON|IECODE_UP_PREFIX))
            {
                s->paint = FALSE;
            }    
        }
        else if (class == IDCMP_MOUSEMOVE)
        {
            if (s->cursX != (mx >> 4) || s->cursY != (my >> 4))
            {
                if (s->paint)
                {
                    s->board.tiles[my >> 4][mx >> 4] = s->tile;
                
                    w->w->RPort->BitMap = s->bm[0];
                
                    bltBitMapRastPort(s->gfx, s->tile << 4, 0 << 4, w->w->RPort, mx & 0xfff0, my & 0xfff0, 16, 16, 0xc0);
                
                    w->w->RPort->BitMap = s->bm[1];
                
                    bltBitMapRastPort(s->gfx, s->tile << 4, 0 << 4, w->w->RPort, mx & 0xfff0, my & 0xfff0, 16, 16, 0xc0);                
                    
                    s->cursX = mx >> 4;
                    s->cursY = my >> 4;
                }
            }
        }    
        else if (class == IDCMP_RAWKEY)
        {
            if (code == 0x45)
            {
                return(TRUE);
            }    
            else if (code == 0x4e)
            {
                s->trigy = 0;
                s->trigx = 1;
            }
            else if (code == 0x4f)
            {
                s->trigy = 0;
                s->trigx = -1;
            }    
            else if (code == 0x4d)
            {
                s->trigy = 1;
                s->trigx = 0;
            }
            else if (code == 0x4c)
            {
                s->trigy = -1;
                s->trigx = 0;
            }                
            else if (code == 0xce || code == 0xcf)
            {
                s->trigx = 0;
            }    
            else if (code == 0xcd || code == 0xcc)
            {
                s->trigy = 0;
            }    
            else if (code >= 0x50 && code <= 0x59)
            {
                s->tile = code - 0x50;
            }
        }
    }    
    return(FALSE);
}    

void initBoard(struct board *b)
{
    WORD x, y;
           
    for (y = 0; y < 14; y++)
    {
        for (x = 0; x < 20; x++)
        {
            if (x == 0 || x == 19 || y == 1 || y == 13)        
            {
                b->tiles[y][x] = T_WALL;
            }
            else
            {    
                b->tiles[y][x] = T_FLOOR;
            }    
        }    
    }    
}

int main(void)
{
    struct loop l = { 0 };
    static struct screen s = { 0 };
    struct window w = { 0 };
    struct TextFont *tf;
    const UBYTE depth = 5;

    struct event *e = l.events + CTRL_C;
    
    s.prepBitMap = prepBitMap;
    
    l.total = 0;
    
    e->func = ctrlC;
    l.total |= e->mask = SIGBREAKF_CTRL_C;
    
    s.l = &l;
    
    initBoard(&s.board);
    
    if (tf = OpenDiskFont(&ta))
    {
        if (openScreen(&s, depth))
        {
            ULONG modeID;
            if (s.gfx = loadPicture("Data/Graphics.iff", &s.s->ViewPort.ColorMap, &s.mask, &modeID))
            {
                MakeScreen(s.s);
                RethinkDisplay();
                if (openWindow(&w, &s))
                {
                    e = l.events + SAFE;
                    e->func = safe;
                    e->obj = &s;
                    l.total |= e->mask = 1L << s.sp->mp_SigBit;
        
                    e = l.events + COPPER;
                    e->func = copper;
                    e->obj = &s;
                    l.total |= e->mask = 1L << s.cop.sigbit;
        
                    e = l.events + IDCMP;
                    e->func = idcmp;
                    e->obj = &w;
                    l.total |= e->mask = 1L << w.w->UserPort->mp_SigBit;
            
                    s.object = 1;
                    s.tile = 1;
                    s.prevPosx[0] = s.prevPosy[0] = s.prevPosx[1] = s.prevPosy[1] = s.posx = s.posy = 64;
                    s.heroX = s.heroY = 4;
    
                    loop(&l);
            
                    closeWindow(&w);
                }          
                FreeBitMap(s.gfx);
                FreeVec(s.mask);
            }    
            closeScreen(&s);
        }    
        CloseFont(tf);
    }    

    return(RETURN_OK);
}
