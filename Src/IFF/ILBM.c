
/* C-1200 Engine Source Code (in C) */

#include <exec/types.h>
#include <exec/memory.h>
#include <datatypes/pictureclass.h>
#include <libraries/iffparse.h>
#include <graphics/gfxmacros.h>

#include <clib/exec_protos.h>
#include <clib/iffparse_protos.h>
#include <clib/graphics_protos.h>

#include "IFF.h"
#include "ILBM.h"

BOOL scanILBM(struct IFFHandle *iff)
{
    if (PropChunk(iff, ID_ILBM, ID_BMHD) == 0)
    {
        if (PropChunk(iff, ID_ILBM, ID_CMAP) == 0)
        {
            if (PropChunk(iff, ID_ILBM, ID_CAMG) == 0)
            {
                if (StopChunk(iff, ID_ILBM, ID_BODY) == 0)
                {
                    if (ParseIFF(iff, IFFPARSE_SCAN) == 0)
                    {
                        return(TRUE);
                    }
                }
            }
        }
    }
    return(FALSE);
}

VOID setCol(ULONG *pal, WORD c, UBYTE red, UBYTE green, UBYTE blue)
{
    pal[(c * 3) + 1] = RGB(red);
    pal[(c * 3) + 2] = RGB(green);
    pal[(c * 3) + 3] = RGB(blue);
}

ULONG *getPal(struct IFFHandle *iff)
{
    struct StoredProperty *sp;

    if (sp = FindProp(iff, ID_ILBM, ID_CMAP))
    {
        LONG size = sp->sp_Size;
        UBYTE *cmap = sp->sp_Data;
        WORD count = size / 3;
        ULONG *pal;
        
        if (pal = AllocVec((size + 2) * sizeof(ULONG), MEMF_PUBLIC))
        {
            WORD c;
            pal[0] = count << 16;
            for (c = 0; c < count; c++)
            {
                setCol(pal, c, cmap[0], cmap[1], cmap[2]);
                cmap += 3;
            }
            pal[size + 1] = 0;
            return(pal);
        }        
    }
    return(NULL);
}

BOOL unpackRow(BYTE **pSrc, BYTE *dest, LONG *pSize, WORD bpr, UBYTE cmp)
{
    BYTE *src = *pSrc;
    LONG size = *pSize;

    if (cmp == cmpNone)
    {
        if (size < bpr)
        {
            return(FALSE);
        }
        CopyMem(src, dest, bpr);
        src += bpr;
        size -= bpr;
    }
    else if (cmp == cmpByteRun1)
    {
        while (bpr > 0)
        {
            BYTE c;
            if (size < 1)
            {
                return(FALSE);
            }
            size--;
            if ((c = *src++) >= 0)
            {
                WORD count = c + 1;
                if (size < count || bpr < count)
                {
                    return(FALSE);                    
                }
                size -= count;
                bpr -= count;
                while (count-- > 0)
                {
                    *dest++ = *src++;
                }
            }
            else if (c != -128)
            {
                WORD count = (-c) + 1;
                BYTE data;
                if (size < 1 || bpr < count)
                {
                    return(FALSE);
                }
                size--;
                bpr -= count;
                data = *src++;
                while (count-- > 0)
                {
                    *dest++ = data;
                }
            }
        }
    }
    else
    {
        return(FALSE);
    }

    *pSrc = src;    
    *pSize = size;
    return(TRUE);
}

VOID initMask(struct BitMap *bm, PLANEPTR mask, WORD width, WORD height, WORD depth)
{
    /* Generate transparent pixels plane */
    struct BitMap aux;
    WORD i;

    BltClear(mask, RASSIZE(width, height), 0);

    InitBitMap(&aux, depth, width, height);    
    for (i = 0; i < depth; i++)
    {
        aux.Planes[i] = mask;
    }

    BltBitMap(bm, 0, 0, &aux, 0, 0, width, height, ABC|ABNC|ANBC, 0xff, NULL);
}

struct BitMap *unpackBitMap(struct IFFHandle *iff, PLANEPTR *mask)
{
    struct BitMap *bm;
    struct BitMapHeader *bmhd;

    if (bmhd = (struct BitMapHeader *)findPropData(iff, ID_ILBM, ID_BMHD))
    {
        WORD width = bmhd->bmh_Width, height = bmhd->bmh_Height;
        UBYTE depth = bmhd->bmh_Depth;
        UBYTE cmp = bmhd->bmh_Compression;
        WORD bpr = RowBytes(width);

        if (*mask = AllocVec(RASSIZE(width, height), MEMF_CHIP))
        {
            if (bm = AllocBitMap(width, height, bmhd->bmh_Depth, BMF_INTERLEAVED, NULL))
            {
                BYTE *buffer;
                LONG size;

                if (buffer = getChunkBuffer(iff, &size))
                {
                    WORD row, plane;
                    PLANEPTR planes[8 + 1], maskPtr = *mask;
                    BYTE *curBuf = buffer;
                    LONG curSize = size;
                    BOOL success = FALSE;

                    for (plane = 0; plane < depth; plane++)
                    {
                        planes[plane] = bm->Planes[plane];
                    }
                    
                    for (row = 0; row < height; row++)
                    {
                        for (plane = 0; plane < depth; plane++)
                        {
                            if (!(success = unpackRow(&curBuf, planes[plane], &curSize, bpr, cmp)))
                            {
                                break;
                            }
                            planes[plane] += bm->BytesPerRow;
                        }
                        if (!success)
                        {
                            break;
                        }
                        if (bmhd->bmh_Masking == mskHasMask)
                        {
                            if (!(success = unpackRow(&curBuf, maskPtr, &curSize, bpr, cmp)))
                            {
                                break;
                            }
                            maskPtr += bpr;
                        }    
                    }
                    FreeMem(buffer, size);
                    if (success)
                    {
                        if (bmhd->bmh_Masking != mskHasMask)
                        {
                            /* Generate mask */
                            initMask(bm, *mask, width, height, depth);
                        }
                        return(bm);
                    }
                }
                FreeBitMap(bm);
            }
    	    FreeVec(*mask);
        }	
    }
    return(NULL);
}
