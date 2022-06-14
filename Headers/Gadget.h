
#include <exec/types.h>

/* Basic gadgets */
BOOL initGadget(struct gadgetData *gd, struct gadgetData *prev, WORD left, WORD top, WORD width, WORD height, WORD ID, VOID (*handle)(struct gadgetData *gd, struct IntuiMessage *msg));
VOID freeGadget(struct gadgetData *gd);
