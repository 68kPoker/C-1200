
#include <libraries/iffparse.h>
#include <clib/graphics_protos.h>
#include <clib/exec_protos.h>

#include "SystemData.h"
#include "System.h"

#include "IFF.h"
#include "ILBM.h"
#include "Screen.h"
#include "Joy.h"

struct TextAttr ta =
{
    "centurion.font", 9,
    FS_NORMAL,
    FPF_DISKFONT|FPF_DESIGNED    
};

long resetSystem(struct systemData *sd, struct programData *pd)
{
    /* Reset system components (libs, devs, screen etc.) */
    struct screenData *screen = &sd->screen;
    struct Rectangle dclip = { 0, 0, SCREEN_BITMAP_WIDTH - 1, SCREEN_BITMAP_HEIGHT - 1 };
    struct IFFHandle *iff;

    if (iff = openIFF("Data1/Warehouse.iff", IFFF_READ))
    {
        if (scanILBM(iff))
        {
            ULONG *pal;

            if (pal = getPal(iff))
            {
                if (screen->gfx = unpackBitMap(iff, &screen->mask)) /* Note: always create a mask */
                {
                    if (screen->bm[0] = AllocBitMap(SCREEN_BITMAP_WIDTH, SCREEN_BITMAP_HEIGHT, screen->gfx->Depth, BMF_DISPLAYABLE|BMF_INTERLEAVED|BMF_CLEAR, NULL))
                    {
                        if (screen->bm[1] = AllocBitMap(SCREEN_BITMAP_WIDTH, SCREEN_BITMAP_HEIGHT, screen->gfx->Depth, BMF_DISPLAYABLE|BMF_INTERLEAVED|BMF_CLEAR, NULL))
                        {
                            if (openScreen(screen, "Warehouse", LORES_KEY, &dclip, pal, &ta))
                            {
                                if (addCop(screen, "Warehouse", 0, 0, NULL))
                                {
                                    if (addDBuf(screen))
                                    {
                                        if (screen->joyIO = openJoy(&screen->joyIE))
                                        {                                            
                                            FreeVec(pal);
                                            closeIFF(iff);
                                            return(TRUE);
                                        }
                                        freeDBuf(screen);
                                    }
                                    remCop(screen);
                                }
                                closeScreen(screen);
                            }
                            FreeBitMap(screen->bm[1]);
                        }
                        FreeBitMap(screen->bm[0]);
                    }
                    FreeBitMap(screen->gfx); /* Remember: free mask also! */
                }
                FreeVec(pal);
            }
        }
        closeIFF(iff);
    }
    return(FALSE);
}

void freeSystem(struct systemData *sd, struct programData *pd)
{
    struct screenData *screen = &sd->screen;

    closeJoy(screen->joyIO);
    freeDBuf(screen);
    remCop(screen);
    closeScreen(screen);
    FreeBitMap(screen->bm[1]);
    FreeBitMap(screen->bm[0]);
    FreeBitMap(screen->gfx);
}
