
/*
** GameC engine
** Libs
*/

#ifndef LIBS_H
#define LIBS_H

#include <exec/types.h>

#define KICK 39

BOOL openROMLibs();
VOID closeROMLibs();

BOOL openDiskLibs();
VOID closeDiskLibs();

#endif /* LIBS_H */
