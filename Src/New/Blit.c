
/* General Blitter functions */

#include <hardware/blit.h>
#include <hardware/custom.h>

#include <clib/graphics_protos.h>

#include "Blit.h"

#ifndef AMIGA
#define __far
#endif

__far extern struct Custom custom;

/* Tile blitting */

/* Check whether tile is in visible ClipRect */

BOOL chkTile(struct RastPort *rp, UBYTE dx, UBYTE dy)
{
    struct Layer *l = rp->Layer;
    struct ClipRect *cr;
    
    if (l == NULL)
    {
        /* Non-layered */
        UBYTE ex = (GetBitMapAttr(rp->BitMap, BMA_WIDTH) >> 4) - 1;
        UBYTE ey = (GetBitMapAttr(rp->BitMap, BMA_HEIGHT) >> 4) - 1;
        
        return(dx <= ex && dy <= ey);
    }    
    
    for (cr = l->ClipRect; cr != NULL; cr = cr->Next)
    {
        struct Rectangle *r = &cr->bounds;
        UBYTE sx, sy, ex, ey;
    
        if (cr->lobs)
            continue;
            
        sx = r->MinX >> 4;
        sy = r->MinY >> 4;
        ex = r->MaxX >> 4;
        ey = r->MaxY >> 4;
        
        if (dx >= sx && dx <= ex && dy >= sy && dy <= ey)
            return(TRUE);                    
    }
    return(FALSE);
}

/* Single layer */

void bltTile(TILE *gfx, struct RastPort *rp, UBYTE dx, UBYTE dy)
{
    struct Layer *l = rp->Layer;
    struct Custom *c = &custom;
        
    dx += l ? l->bounds.MinX >> 4 : 0;
    dy += l ? l->bounds.MinY >> 4 : 0;
    
    if (chkTile(rp, dx, dy))
    {    
        OwnBlitter();
        WaitBlit();
        
        c->bltcon0 = SRCA | DEST | A_TO_D;
        c->bltcon1 = 0;
        c->bltapt  = gfx->gfx->Planes[0] + (gfx->gfx->BytesPerRow * (gfx->gfxY << 4)) + (gfx->gfxX << 1);
        c->bltdpt  = rp->BitMap->Planes[0] + (rp->BitMap->BytesPerRow * (dy << 4)) + (dx << 1);
        c->bltamod = gfx->gfx->Planes[1] - gfx->gfx->Planes[0] - 2;
        c->bltdmod = rp->BitMap->Planes[1] - rp->BitMap->Planes[0] - 2;
        c->bltafwm = 0xffff;
        c->bltalwm = 0xffff;
        c->bltsize = ((16 * rp->BitMap->Depth) << HSIZEBITS) | 1;
    
        DisownBlitter();                        
   }
}

/* Dual layer */

void bltAddTile(TILE *floor, TILE *object, BYTE offset, UBYTE orient, PLANEPTR mask, struct RastPort *rp, UBYTE dx, UBYTE dy)
{
    struct Layer *l = rp->Layer;
    struct Custom *c = &custom;
        
    dx += l ? l->bounds.MinX >> 4 : 0;
    dy += l ? l->bounds.MinY >> 4 : 0;
    
    if (chkTile(rp, dx, dy))
    {    
        UBYTE p, depth = rp->BitMap->Depth;
        WORD maskbpr = RowBytes(GetBitMapAttr(object->gfx, BMA_WIDTH));
        BYTE shift = (orient == HORIZ ? offset : 0);
        WORD desc = 0;
        UBYTE height = 16;
        
        LONG maskOffset = (maskbpr * (object->gfxY << 4)) + (object->gfxX << 1);
        LONG objectOffset = (object->gfx->BytesPerRow * (object->gfxY << 4)) + (object->gfxX << 1);
        LONG floorOffset = (floor->gfx->BytesPerRow * (floor->gfxY << 4)) + (floor->gfxX << 1);
        LONG destOffset = (rp->BitMap->BytesPerRow * (dy << 4)) + (dx << 1);
        
        if (orient == HORIZ && offset < 0)
        {
            desc = BLITREVERSE;
            
            maskOffset += (maskbpr * 15);
            objectOffset += (object->gfx->BytesPerRow * 15);
            floorOffset += (floor->gfx->BytesPerRow * 15);
            destOffset += (rp->BitMap->BytesPerRow * 15);
            
            shift = -offset;
        }
        else if (orient == VERT && offset != 0)
        {
            UBYTE top = 0;
            
            if (offset > 0)
            {
                top = offset;
                height -= top;
                
                floorOffset += (floor->gfx->BytesPerRow * top);
                destOffset += (rp->BitMap->BytesPerRow * top);
            }
            else
            {
                top = -offset;
                height = 16 + offset;
                
                maskOffset += maskbpr * top;
                objectOffset += (object->gfx->BytesPerRow * top);                
            }
        }
        
        OwnBlitter();
        
        for (p = 0; p < depth; p++)
        {
            WaitBlit();
            
            c->bltcon0 = SRCA | SRCB | SRCC | DEST | 0xca | (shift << ASHIFTSHIFT);
            c->bltcon1 = desc | (shift << BSHIFTSHIFT);
            c->bltapt  = mask + maskOffset;
            c->bltbpt  = object->gfx->Planes[p] + objectOffset;
            c->bltcpt  = floor->gfx->Planes[p] + floorOffset;
            c->bltdpt  = rp->BitMap->Planes[p] + destOffset;
            c->bltamod = maskbpr - 2;
            c->bltbmod = object->gfx->BytesPerRow - 2;
            c->bltcmod = floor->gfx->BytesPerRow - 2;
            c->bltdmod = rp->BitMap->BytesPerRow - 2;
            c->bltafwm = 0xffff;
            c->bltalwm = offset >= 0 ? 0xffff << shift : 0xffff >> shift;
            c->bltsize = (height << HSIZEBITS) | 1;            
        }    
        
        DisownBlitter();
    }    
}
