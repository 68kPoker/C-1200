
/* C-1200 Engine Source Code (in C) */

#include "Libs.h"
#include "IFF.h"
#include "Screen.h"

struct gfxData
{
    struct BitMap *gfx;
    PLANEPTR mask;
    ULONG *pal;
};

struct mainData
{
    struct gfxData gd;
    struct screenData sd;
};

/* Main function. All setup is done - the Screen is either obtained or opened 
 * (all GearWorks apps and games work on one public screen with double-buffering).
 * The graphics are loaded, colors are obtained, audio channels are allocated. Then the signal
 * loop with Wait() is called and it sends signals to all receivers. */

WORD setupGfx(struct gfxData *gd)
{
    struct IFFHandle *iff;

    if (iff = openIFF("Data1/Gfx1.iff", IFFF_READ))
    {
        if (scanILBM(iff))
        {
            if (gd->pal = getPal(iff))
            {
                if (gd->gfx = unpackBitMap(iff, &gd->mask))
                {
                    closeIFF(iff);
                    return(TRUE);                    
                }
                FreeVec(gd->pal);
            }
        }
        closeIFF(iff);
    }
    return(FALSE);
}

WORD setupScreen(struct screenData *sd, struct gfxData *gd)
{
    if (sd->bm[0] = AllocBitMap(320, 256, depth, BMF_DISPLAYABLE|BMF_INTERLEAVED, NULL))
    {
        if (sd->bm[1] = AllocBitMap(320, 256, depth, BMF_DISPLAYABLE|BMF_INTERLEAVED, NULL))
        {            
            if (openScreen(sd, "Magazyn", LORES_KEY, &dclip, gd->pal, &ta))
            {
                if (addCop(sd, "Magazyn", 0, 0, NULL))
                {
                    if (addDBuf(sd))
                    {
                        return(TRUE);
                    }
                    remCop(sd);
                }
                closeScreen(sd);
            }
            FreeBitMap(sd->bm[1]);
        }
        FreeBitMap(sd->bm[0]);
    }
    return(FALSE);
}

int main(void)
{
    static struct mainData md;

    if (openROMLibs()) /* Open ROM libs at first place */
    {
        if (openDiskLibs()) /* Open required disk libs at second place */
        {
            if (setupGfx(&md.gd))
            {
                if (setupScreen(&md.sd))
                {
                    closeScreen();
                }
                closeGfx();
            }
            closeDiskLibs();
        }
        closeROMLibs();
    }
    return(0);
}
