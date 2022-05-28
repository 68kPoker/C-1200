
/* Blitter.c - Own Blitter functions */

#include <assert.h>

#include <hardware/custom.h>
#include <graphics/rastport.h>
#include <clib/graphics_protos.h>

#include "Blitter.h"

__far extern struct Custom custom;

void bltBitMap(struct BitMap *sbm, WORD sx, WORD sy, struct BitMap *dbm, WORD dx, WORD dy, WORD width, WORD height, UBYTE minterm)
{
    struct Custom *c = &custom;
    LONG soffset = 0, doffset = 0;
    BOOL desc = FALSE;

    ULONG flags1 = GetBitMapAttr(sbm, BMA_FLAGS), flags2 = GetBitMapAttr(dbm, BMA_FLAGS);

    assert((flags1 & BMF_INTERLEAVED) && (flags2 & BMF_INTERLEAVED));

    if (sbm == dbm)
    {
        /* Same bitmap */

        if (!(sx > dx + width - 1 || dx > sx + width - 1 || sy > dy + height - 1 || dy > sy + height - 1))
        {
            /* Overlapped */

            if ((dy - sy) > 0 || ((dy - sy) == 0 && (dx - sx) > 0))
            {
                /* Descending */
                desc = TRUE;

                soffset = (sbm->BytesPerRow * (height)) - (sbm->Planes[1] - sbm->Planes[0]) + (width >> 3) - 2;
                doffset = (dbm->BytesPerRow * (height)) - (dbm->Planes[1] - dbm->Planes[0]) + (width >> 3) - 2;
            }
        }
    }

    OwnBlitter();

    WaitBlit();

    c->bltcon0 = 0x09f0;
    c->bltcon1 = desc ? BLITREVERSE : 0;
    c->bltapt  = sbm->Planes[0] + (sbm->BytesPerRow * sy) + ((sx >> 4) << 1) + soffset;
    c->bltdpt  = dbm->Planes[0] + (dbm->BytesPerRow * dy) + ((dx >> 4) << 1) + doffset;
    c->bltamod = sbm->Planes[1] - sbm->Planes[0] - (width >> 3);
    c->bltdmod = dbm->Planes[1] - dbm->Planes[0] - (width >> 3);
    c->bltafwm = 0xffff;
    c->bltalwm = 0xffff;
    c->bltsizv = height * dbm->Depth;
    c->bltsizh = width >> 4;

    DisownBlitter();
}

void bltBitMapRastPort(struct BitMap *sbm, WORD sx, WORD sy, struct RastPort *drp, WORD dx, WORD dy, WORD width, WORD height, UBYTE minterm)
{
    struct Layer *dl = drp->Layer;
    struct BitMap *dbm = drp->BitMap;

    if (dl)
    {
        struct ClipRect *dcr;
        WORD adx = dx + dl->bounds.MinX;
        WORD ady = dy + dl->bounds.MinY;

        LockLayer(0, dl);

        for (dcr = dl->ClipRect; dcr != NULL; dcr = dcr->Next)
        {
            WORD x0 = MAX(dcr->bounds.MinX, adx);
            WORD x1 = MIN(dcr->bounds.MaxX, adx + width - 1);
            WORD y0 = MAX(dcr->bounds.MinY, ady);
            WORD y1 = MIN(dcr->bounds.MaxY, ady + height - 1);

            WORD ox = x0 - adx;
            WORD oy = y0 - ady;

            if (dcr->lobs)  continue;

            if (x0 > x1 || y0 > y1) continue;

            bltBitMap(sbm, sx + ox, sy + oy, dbm, x0, y0, x1 - x0 + 1, y1 - y0 + 1, 0xc0);
        }
        UnlockLayer(dl);
    }
    else
    {
        bltBitMap(sbm, sx, sy, dbm, dx, dy, width, height, 0xc0);
    }
}

/* Fast Blit from source RastPort to destination RastPort */

void clipBlit(struct RastPort *srp, WORD sx, WORD sy, struct RastPort *drp, WORD dx, WORD dy, WORD width, WORD height, UBYTE minterm)
{
    struct Layer *sl = srp->Layer;
    struct BitMap *sbm = srp->BitMap;

    if (sl)
    {
        struct ClipRect *scr;
        WORD asx = sx + sl->bounds.MinX;
        WORD asy = sy + sl->bounds.MinY;

        for (scr = sl->ClipRect; scr != NULL; scr = scr->Next)
        {
            WORD x0 = MAX(scr->bounds.MinX, asx);
            WORD x1 = MIN(scr->bounds.MaxX, asx + width - 1);
            WORD y0 = MAX(scr->bounds.MinY, asy);
            WORD y1 = MIN(scr->bounds.MaxY, asy + height - 1);

            WORD ox = x0 - asx;
            WORD oy = y0 - asy;

            if (scr->lobs)  continue;

            if (x0 > x1 || y0 > y1) continue;

            bltBitMapRastPort(sbm, x0, y0, drp, dx + ox, dy + oy, x1 - x0 + 1, y1 - y0 + 1, 0xc0);
        }
    }
    else
    {
        bltBitMapRastPort(sbm, sx, sy, drp, dx, dy, width, height, 0xc0);
    }
}
