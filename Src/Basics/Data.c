
/* C-1200 */

/* Data to be loaded from IFF */

#include "Basics.h"

#include "IFF.h"
#include "ILBM.h"

/* Implement in Data.c */
BOOL initGraphics(struct GfxPath *path, struct GfxData *gfx, struct GfxTempData *temp)
{
    struct IFFHandle *iff;

    path->gfx = gfx;

    if (iff = openIFF(temp->name, IFFF_READ))
    {
        if (scanILBM(iff))
        {
            if (temp->RGB = getPal(iff))
            {
                if (gfx->bm = unpackBitMap(iff, &gfx->mask))
                {
                    closeIFF(iff);
                    return(TRUE);
                }
                FreeVec(temp->RGB);
            }            
        }
        closeIFF(iff);
    }
    return(FALSE);
}

VOID freeGraphicsTemp(struct GfxTempData *temp)
{
    if (temp->RGB)
    {
        FreeVec(temp->RGB);
    }
}

VOID freeGraphics(struct GfxData *gfx)
{
    FreeBitMap(gfx->bm);    
}
