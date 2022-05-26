
/*
** GameX engine
** Src > Init > Libs
*/

#include <exec/libraries.h>

#include <clib/exec_protos.h>

#include "Libs.h"

struct Library
    *IntuitionBase,
    *GfxBase,
    *DiskfontBase,
    *IFFParseBase;

BOOL openLibs()
{
    if (IntuitionBase = OpenLibrary("intuition.library", KICK))
    {
        if (GfxBase = OpenLibrary("graphics.library", KICK))
        {
            if (DiskfontBase = OpenLibrary("diskfont.library", KICK))
            {
                if (IFFParseBase = OpenLibrary("iffparse.library", KICK))
                {
                    return(TRUE);
                }
                CloseLibrary(DiskfontBase);
            }
            CloseLibrary(GfxBase);
        }
        CloseLibrary(IntuitionBase);
    }
    return(FALSE);
}

VOID closeLibs()
{
    CloseLibrary(IFFParseBase);
    CloseLibrary(DiskfontBase);
    CloseLibrary(GfxBase);
    CloseLibrary(IntuitionBase);
}
