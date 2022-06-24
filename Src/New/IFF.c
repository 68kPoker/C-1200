
/* General IFF functions */

#include <dos/dos.h>
#include <libraries/iffparse.h>
#include <exec/memory.h>
#include <datatypes/pictureclass.h>

#include <clib/dos_protos.h>
#include <clib/iffparse_protos.h>
#include <clib/graphics_protos.h>
#include <clib/exec_protos.h>

#include "IFF.h"

#define RGB(c) ((c)|((c)<<8)|((c)<<16)|((c)<<24))
#define RowBytes(w) ((((w)+15)>>4)<<1)

void readColors(struct ColorMap *cm, UBYTE *cmap, WORD colors)
{
    WORD i;
    
    for (i = 0; i < colors; i++)
    {
        UBYTE red = *cmap++, green = *cmap++, blue = *cmap++;
        
        SetRGB32CM(cm, i, RGB(red), RGB(green), RGB(blue));
    }
}    

BOOL unpackRow(BYTE **srcPtr, LONG *sizePtr, BYTE *dest, WORD bpr, UBYTE cmp)
{
    BYTE *src = *srcPtr;
    LONG size = *sizePtr;

    if (cmp == cmpNone)
    {
        if (size < bpr)
        {   
            return(FALSE);
        }    
        size -= bpr;
        CopyMem(src, dest, bpr);
        src += bpr;
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

    *srcPtr = src;
    *sizePtr = size;    
    return(TRUE);
}

VOID prepMask(struct BitMap *bm, PLANEPTR mask, WORD w, WORD h, UBYTE d)
{
    struct BitMap aux;
    UBYTE i;
    
    InitBitMap(&aux, d, w, h);
    
    for (i = 0; i < d; i++)
    {
        aux.Planes[i] = mask;
    }
    
    BltBitMap(bm, 0, 0, &aux, 0, 0, w, h, ABC|ABNC|ANBC, 0xff, NULL);
}

struct BitMap *readPicture(struct IFFHandle *iff, struct ColorMap **cm, PLANEPTR *mask)
{
    struct StoredProperty *sp;
    struct ContextNode *cn;
    struct BitMapHeader *bmhd;
    struct BitMap *bm;
    BOOL customCm = (*cm) != NULL;
    
    if (cn = CurrentChunk(iff))
    {
        if (sp = FindProp(iff, ID_ILBM, ID_BMHD))
        {
            bmhd = (struct BitMapHeader *)sp->sp_Data;
            WORD w = bmhd->bmh_Width;
            WORD h = bmhd->bmh_Height;
            UBYTE d = bmhd->bmh_Depth;
            UBYTE cmp = bmhd->bmh_Compression;
            UBYTE msk = bmhd->bmh_Masking;
            
            if (sp = FindProp(iff, ID_ILBM, ID_CMAP))
            {
                WORD colors = sp->sp_Size / 3;
                if (!customCm)
                {
                    *cm = GetColorMap(colors);
                }
                    
                if (*cm)
                {
                    readColors(*cm, sp->sp_Data, colors);
            
                    if (bm = AllocBitMap(w, h, d, BMF_INTERLEAVED, NULL))
                    {
                        if (*mask = AllocVec(RASSIZE(w, h), MEMF_CHIP|MEMF_CLEAR))
                        {
                            BYTE *buffer;
                            
                            if (buffer = AllocMem(cn->cn_Size, MEMF_PUBLIC))
                            {
                                if (ReadChunkBytes(iff, buffer, cn->cn_Size) == cn->cn_Size)
                                {
                                    WORD i, j;
                                    WORD bpr = RowBytes(w);
                                    PLANEPTR planes[8], maskPtr = *mask;
                                    LONG curSize = cn->cn_Size;
                                    BYTE *curBuf = buffer;
                                    BOOL success = FALSE;
                            
                                    for (i = 0; i < d; i++)
                                    {
                                        planes[i] = bm->Planes[i];
                                    }    
                            
                                    for (j = 0; j < h; j++)
                                    {
                                        for (i = 0; i < d; i++)
                                        {
                                            if (!(success = unpackRow(&curBuf, &curSize, planes[i], bpr, cmp)))
                                            {
                                                break;
                                            }    
                                            planes[i] += bm->BytesPerRow;
                                        }    
                                        if (!success)
                                        {
                                            break;
                                        }    
                                        if (msk == mskHasMask)
                                        {
                                            if (!(success = unpackRow(&curBuf, &curSize, maskPtr, bpr, cmp)))
                                            {
                                                break;
                                            }    
                                            maskPtr += bpr;
                                        }    
                                    }    
                                    if (success)
                                    {
                                        if (msk != mskHasMask)
                                        {
                                            prepMask(bm, *mask, w, h, d);
                                        }    
                                        FreeMem(buffer, cn->cn_Size);
                                        return(bm);
                                    }    
                                }
                                FreeMem(buffer, cn->cn_Size);
                            }
                            FreeVec(*mask);
                        }
                        FreeBitMap(bm);
                    }   
                    if (!customCm)
                    {
                        FreeColorMap(*cm);
                    }    
                }
            }        
        }    
    }
    return(NULL);    
}

struct BitMap *loadPicture(STRPTR name, struct ColorMap **cm, PLANEPTR *mask, ULONG *modeID)
{
    struct IFFHandle *iff;
    struct BitMap *bm = NULL;
    
    if (iff = AllocIFF())
    {
        if (iff->iff_Stream = Open(name, MODE_OLDFILE))
        {
            InitIFFasDOS(iff);
            if (OpenIFF(iff, IFFF_READ) == 0)
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
                                    struct StoredProperty *sp;
                                    
                                    if (sp = FindProp(iff, ID_ILBM, ID_CAMG))
                                    {
                                        *modeID = *(ULONG *)sp->sp_Data;
                                        bm = readPicture(iff, cm, mask);
                                    }    
                                }    
                            }    
                        }        
                    }    
                }    
                CloseIFF(iff);
            }    
            Close(iff->iff_Stream);
        }    
        FreeIFF(iff);
    }
    return(bm);
}
