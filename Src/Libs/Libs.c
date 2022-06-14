
/* C-1200 Engine Source Code (in C) */

#include <exec/libraries.h>
#include <clib/exec_protos.h>

#include "Libs.h"

struct Library
    *IntuitionBase,
    *GfxBase,
    *DiskfontBase,
    *IFFParseBase;

BOOL openROMLibs()
{
    if (IntuitionBase = OpenLibrary("intuition.library", KICK))
    {
        if (GfxBase = OpenLibrary("graphics.library", KICK))
        {
            return(TRUE);
        }
        CloseLibrary(IntuitionBase);
    }
    printf("Couldn't open required V39 ROM libraries!\n");
    return(FALSE);
}

BOOL openDiskLibs()
{
    if (DiskfontBase = OpenLibrary("diskfont.library", KICK))
    {
        if (IFFParseBase = OpenLibrary("iffparse.library", KICK))
        {
            return(TRUE);
        }
        else
            printf("Couldn't open V39 iffparse.library!\n");
        CloseLibrary(DiskfontBase);
    }
    else
        printf("Couldn't open V39 diskfont.library!\n");
    return(FALSE);
}

VOID closeDiskLibs()
{
    CloseLibrary(IFFParseBase);
    CloseLibrary(DiskfontBase);
}

VOID closeROMLibs()
{
    CloseLibrary(GfxBase);
    CloseLibrary(IntuitionBase);
}

/** EOF **/
