
/*
** C-1200 engine
** Src > Init > Windows
*/

#ifndef WINDOWS_H
#define WINDOWS_H

#include <exec/types.h>

BOOL openWindow(struct windowData *wd, struct screenData *sd, ULONG tag1, ...);
VOID closeWindow(struct windowData *wd);

#endif /* WINDOWS_H */
