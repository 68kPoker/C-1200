
/*
** C-1200 engine
** Src > Init > IFF
**
** Interchange-File-Format general handler
*/

#include <dos/dos.h>
#include <libraries/iffparse.h>
#include <exec/memory.h>

#include <clib/dos_protos.h>
#include <clib/iffparse_protos.h>
#include <clib/exec_protos.h>

#include "IFF.h"

struct IFFHandle *openIFF(STRPTR name, LONG mode)
{
    struct IFFHandle *iff;
    LONG dosModes[] =
    {
        MODE_OLDFILE,
        MODE_NEWFILE
    };

    if (iff = AllocIFF())
    {
        if (iff->iff_Stream = Open(name, dosModes[mode]))
        {
            InitIFFasDOS(iff);
            if (OpenIFF(iff, mode) == 0)
            {
                return(iff);
            }
            Close(iff->iff_Stream);
        }
        FreeIFF(iff);
    }
    return(NULL);
}

VOID closeIFF(struct IFFHandle *iff)
{
    CloseIFF(iff);
    Close(iff->iff_Stream);
    FreeIFF(iff);
}

BYTE *findPropData(struct IFFHandle *iff, ULONG type, ULONG ID)
{
    struct StoredProperty *sp;
    
    if (sp = FindProp(iff, type, ID))
    {
        return(sp->sp_Data);
    }
    return(NULL);
}

BYTE *getChunkBuffer(struct IFFHandle *iff, LONG *pSize)
{
    struct ContextNode *cn;

    if (cn = CurrentChunk(iff))
    {
        LONG size = cn->cn_Size;
        BYTE *buffer;

        if (buffer = AllocMem(size, MEMF_PUBLIC))
        {
            if (ReadChunkBytes(iff, buffer, size) == size)
            {
                *pSize = size;
                return(buffer);
            }
            FreeMem(buffer, size);
        }
    }
    return(NULL);
}
