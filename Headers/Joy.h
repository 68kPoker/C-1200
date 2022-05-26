
/*
** GameX engine
** Src > Init > Devs
*/

#ifndef JOY_H
#define JOY_H

#include <exec/types.h>

#define TIMEOUT 50

struct IOStdReq *openJoy(struct InputEvent *ie);
VOID closeJoy(struct IOStdReq *io);
VOID readJoy(struct IOStdReq *io, struct InputEvent *ie);
VOID clearIO(struct IOStdReq *io);

#endif /* JOY_H */
