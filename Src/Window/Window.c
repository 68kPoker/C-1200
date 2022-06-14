
/*
** C-1200 engine
** Src > Init > Windows
**
** Windows and basic gadget handler
*/

#include <intuition/intuition.h>
#include <exec/memory.h>

#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#include <clib/utility_protos.h>

#include "WindowData.h"
#include "Window.h"
#include "ScreenData.h"

BOOL openWindow(struct windowData *wd, struct screenData *sd, ULONG tag1, ...)
{
    struct Window *w;
    
    if (wd->w = w = OpenWindowTags(NULL,
        WA_CustomScreen,    sd->s,
        WA_Borderless,      TRUE,
        WA_RMBTrap,         TRUE,
        WA_SimpleRefresh,   TRUE,
        WA_BackFill,        LAYERS_NOBACKFILL,
        WA_ReportMouse,     TRUE,
        TAG_MORE,           &tag1))
    {
    	wd->done = FALSE;
    
        w->UserData = (APTR)wd;

        return(TRUE);
    }
    return(FALSE);
}

VOID closeWindow(struct windowData *wd)
{
    CloseWindow(wd->w);
}
