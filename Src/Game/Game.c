
#include <stdio.h>

#include <dos/dos.h>
#include <intuition/intuition.h>

#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>

#include "Libs.h"
#include "IFF.h"
#include "ILBM.h"
#include "Screen.h"
#include "Joy.h"
#include "Windows.h"
#include "Edit.h"
#include "Loop.h"

struct TextAttr ta = 
{
    "centurion.font", 9, FS_NORMAL, FPF_DISKFONT|FPF_DESIGNED
};

struct Rectangle dclip =
{
    0, 0, 319, 255
};

VOID prepBitMap(struct BitMap *bm, struct BitMap *gfx)
{
    struct RastPort rp;

    InitRastPort(&rp);
    rp.BitMap = bm;

    SetRast(&rp, 0);
}

int main(int argc, char **argv)
{
    if (openLibs())
    {
        struct IFFHandle *iff;
        if (iff = openIFF("Data/Graphics.iff", IFFF_READ))
        {
            if (scanILBM(iff))
            {
                ULONG *pal;
                if (pal = getPal(iff))
                {
                    struct BitMap *gfx;
                    if (gfx = unpackBitMap(iff))
                    {
                        struct BitMap *bm[2];

                        if (bm[0] = AllocBitMap(320, 256, gfx->Depth, BMF_DISPLAYABLE|BMF_INTERLEAVED, NULL))
                        {
                            if (bm[1] = AllocBitMap(320, 256, gfx->Depth, BMF_DISPLAYABLE|BMF_INTERLEAVED, NULL))
                            {
                                static struct screenData sd;

                                prepBitMap(bm[0], gfx);
                                prepBitMap(bm[1), gfx);

                                if (openScreen(&sd, "Warehouse", PAL_MONITOR_ID|LORES_KEY, &dclip, bm, pal, &ta))
                                {
                                    if (addCop(&sd, "Warehouse", 0, 0, NULL))
                                    {
                                        if (addDBuf(&sd))
                                        {
                                            static struct windowData wd;

                                            if (openWindow(&wd, &sd, 
                                                WA_Left, 0, 
                                                WA_Top, 0,
                                                WA_Width, sd.s->Width,
                                                WA_Height, sd.s->Height,
                                                WA_IDCMP, IDCMP_RAWKEY|IDCMP_GADGETDOWN|IDCMP_MOUSEMOVE,                                                
                                                TAG_DONE))
                                            {
                                                struct IOStdReq *joyIO;
                                                static struct InputEvent joyIE;

                                                if (joyIO = openJoy(&joyIE))
                                                {
                                                    eventLoop(&sd, &wd, joyIO, &joyIE, gfx);
                                                    closeJoy(joyIO);
                                                }
                                                else
                                                    printf("Couldn't obtain joystick!\n");
                                                closeWindow(&wd);
                                            }
                                            remDBuf(&sd);
                                        }
                                        remCop(&sd);
                                    }                                    
                                    closeScreen(&sd);
                                }
                                FreeBitMap(bm[1]);
                            }
                            FreeBitMap(bm[0]);
                        }
                        FreeBitMap(gfx);
                    }
                    FreeVec(pal);
                }
            }
            closeIFF(iff);
        }
        else
            printf("Couldn't open graphics file!\n");
        closeLibs();
    }
    else
        printf("Couldn't open required libraries V39!\n");
    return(RETURN_OK);
}
