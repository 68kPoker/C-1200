
/*
** GameX engine
** Src > Events
*/

#include <intuition/intuition.h>

#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>

#include "Loop.h"
#include "Windows.h"
#include "Screen.h"

VOID eventLoop(struct screenData *sd, struct windowData *wd, UWORD *board)
{
    struct IOStdReq *joyIO = sd->joyIO;
    struct InputEvent *joyIE = &sd->joyIE;
    struct RastPort *rp = &sd->s->RastPort;
    struct gfxData *gfxData = (struct gfxData *)rp->RP_User;
    struct BitMap *gfx = sd->gfx;

    ULONG sigMasks[EID_COUNT] =
    {
        1L << sd->safePort->mp_SigBit,
        1L << sd->cd.sigBit,
        1L << wd->w->UserPort->mp_SigBit,
        1L << joyIO->io_Message.mn_ReplyPort->mp_SigBit  
    }, total = 0;
    WORD i;
    BOOL done = FALSE;

    for (i = 0; i < EID_COUNT; i++)
    {
        total |= sigMasks[i];
    }

    while (!done)
    {
        ULONG result = Wait(total);

        if (result & sigMasks[EID_SAFE_TO_DRAW])
        {
            safeToDraw(sd);
            /* Draw here */
        }

        if (result & sigMasks[EID_SAFE_TO_CHANGE])
        {
            /* Swap buffers here */
            /* swapBuf(sd); */
        }

        if (result & sigMasks[EID_USER])
        {
            struct IntuiMessage *msg, msgCopy;
            struct gadgetData *active = wd->activeGad;

            while ((!done) && (msg = (struct IntuiMessage *)GetMsg(wd->w->UserPort)))
            {            
            	msgCopy = *msg;
            	ReplyMsg((struct Message *)msg);
            	
            	msg = &msgCopy;
            	
            	if (msg->Class == IDCMP_REFRESHWINDOW)
            	{
            		BeginRefresh(wd->w);
            		prepBitMap(board, wd->w->RPort, sd->gfx);
            		EndRefresh(wd->w, TRUE);
            	}           
            	if (msg->Class == IDCMP_GADGETDOWN)
            	{
            		struct Gadget *gad = (struct Gadget *)msg->IAddress;
            		struct gadgetData *gd = (struct gadgetData *)gad->UserData;
            		
            		if (gd->handleIDCMP)
            		{
            			gd->handleIDCMP(gd, msg);
            		}	
            	}	
                else if (wd->activeGad)
                {
                    wd->activeGad->handleIDCMP(active, msg);
                }
                else if (msg->Class == IDCMP_RAWKEY)
                {
                    if (msg->Code == ESC_KEY)
                    {
                        wd->done = TRUE;
                    }
                }
                done = wd->done;
            }

            if (done)
            {
                break;
            }
        }

        if (result & sigMasks[EID_JOYSTICK])
        {
            /* Handle joystick */
        }
    }
}
