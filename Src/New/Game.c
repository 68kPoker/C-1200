
#include <intuition/screens.h>
#include <intuition/intuition.h>

#include <clib/intuition_protos.h>
#include <clib/graphics_protos.h>
#include <clib/exec_protos.h>

#include "Board.h"
#include "Blitter.h"

enum
{
    SAFE,
    COPPER_SIG,
    IDCMP,
    SIGNALS
};    

void drawPanel(SCREEN *s, BOARD *b, struct Window *w)
{
    static BOOL drawn[2] = { 0 };
    
    if (!drawn[s->frame])
    {
        bltBitMapRastPort(s->gfx, 0, 224, &s->s->RastPort, 0, 0, 320, 32, 0xc0);
        drawn[s->frame] = TRUE;
    }
}

void loop(SCREEN *s, BOARD *b, struct Window *w)
{
    ULONG masks[SIGNALS];
    BOOL done = FALSE;
    ULONG total;
    
    setObject(b, 0, O_HERO, 30, 5, 5);
    setObject(b, 1, O_BOX, 3, 7, 7);    
    setObject(b, 2, O_BOX, 3, 10, 7);        
    
    total = masks[SAFE] = 1L << s->sp->mp_SigBit;
    total |= masks[COPPER_SIG] = 1L << s->cop.sigBit;
    total |= masks[IDCMP] = 1L << w->UserPort->mp_SigBit;

    while (!done)
    {
        ULONG result = Wait(total);
        
        if (result & masks[IDCMP])
        {
            struct IntuiMessage *msg;
            while (msg = (struct IntuiMessage *)GetMsg(w->UserPort))
            {
                ULONG class = msg->Class;
                WORD code = msg->Code;
                WORD mx = msg->MouseX;
                WORD my = msg->MouseY;
                
                ReplyMsg((struct Message *)msg);
                
                if (class == IDCMP_RAWKEY)
                {
                    if (code == 0x45)
                    {
                        done = TRUE;
                    }
                    else if (code == 0x4c)
                    {
                        b->obj[0].ty = -1;
                    }    
                    else if (code == 0x4d)
                    {
                        b->obj[0].ty = 1;
                    }    
                    else if (code == 0x4e)
                    {
                        b->obj[0].tx = 1;
                    }    
                    else if (code == 0x4f)
                    {
                        b->obj[0].tx = -1;
                    }                        
                    else if ((code == 0xcc || code == 0xcd) && b->obj[0].ty)
                    {
                        b->obj[0].ty = 0;
                    }    
                    else if ((code == 0xce || code == 0xcf) && b->obj[0].tx)
                    {
                        b->obj[0].tx = 0;
                    }    
                }        
            }    
        }

        if (result & masks[SAFE])        
        {
            if (!s->safe)
            {
                while (!GetMsg(s->sp))
                    WaitPort(s->sp);
                s->safe = TRUE;        
            }    
            
            s->s->RastPort.BitMap = s->bm[s->frame];
            
            drawPanel(s, b, w);
            
            scanObjects(b);
            drawBoard(b);
        }
        
        if (result & masks[COPPER_SIG])
        {
            if (s->safe)
            {
                WaitBlit();
                ChangeVPBitMap(s->cop.vp, s->bm[s->frame], s->dbi);
            
                b->frame = s->frame ^= 1;
            
                s->safe = FALSE;            
            }
        }
    }    
}

int main(void)
{
    SCREEN s;
    static BOARD b = { 0 };
    
    if (openScreen(&s, "Data1/Graphics.iff"))
    { 
        WORD x, y, i;
        struct Window *w;
        
        b.frame = s.frame;
        
        b.s = &s;
        
        for (y = 2; y < 16; y++)
        {
            for (x = 0; x < 20; x++)
            {
                struct tile *t = &b.tiles[y][x];
                if (x == 0 || x == 19 || y == 2 || y == 15)
                {
                    t->kind = T_WALL;
                    t->type = 1;
                }                    
                else if (y == 10)
                {   
                    t->kind = T_CRACKED;
                    t->type = 5;
                }
                else    
                {
                    t->kind = T_FLOOR;
                    t->type = 2;    
                }    
                t->update[0] = t->update[1] = TRUE;
            }    
        }    
        
        if (w = OpenWindowTags(NULL,
            WA_CustomScreen,    s.s,
            WA_Left,            0,
            WA_Top,             0,
            WA_Width,           s.s->Width,
            WA_Height,          s.s->Height,
            WA_Borderless,      TRUE,
            WA_RMBTrap,         TRUE,
            WA_Activate,        TRUE,
            WA_SimpleRefresh,   TRUE,
            WA_BackFill,        LAYERS_NOBACKFILL,
            WA_IDCMP,           IDCMP_RAWKEY,
            TAG_DONE))
        {    
            loop(&s, &b, w);
            
            CloseWindow(w);
        }        
        closeScreen(&s);
    }
    return(0);    
}    
