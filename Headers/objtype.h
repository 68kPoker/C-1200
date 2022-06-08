
/* 
** Obj_types.h
*/

#include <exec/types.h>

enum { NONE, HERO, BOX, OBJS };

extern void calcObjTypeGfx(int *gfx);

typedef int (*handleObjType[OBJS])(int order, int pos);

extern void setObjHandlers(handleObjType handle);
