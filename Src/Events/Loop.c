
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

VOID eventLoop(struct screenData *sd, struct windowData *wd, struct IOStdReq *joyIO, struct InputEvent *joyIE, struct BitMap *gfx)
{
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
            swapBuf(sd);
        }

        if (result & sigMasks[EID_USER])
        {
            struct IntuiMessage *msg;
            struct gadgetData *active = wd->activeGadget;

            while ((!done) && (msg = (struct IntuiMessage *)GetMsg(wd->w->UserPort)))
            {   
                if (msg->Class == IDCMP_GADGETDOWN)         
                {
                    struct Gadget *gad = (struct Gadget *)msg->IAddress;
                    struct gadgetData *gd = (struct gadgetData *)gad->UserData;

                    if (gd->handleIDCMP)
                    {
                        gd->handle(gd, msg);
                    }
                }
                else if (active)
                {
                    active->handleIDCMP(active, msg);
                }
                else if (msg->Class == IDCMP_RAWKEY)
                {
                    if (msg->Code == ESC_KEY)
                    {
                        wd->done = TRUE;
                    }
                }
                done = wd->done;
                ReplyMsg((struct Message *)msg);
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
