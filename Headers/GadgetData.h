
#include <exec/types.h>

struct gadgetData 
{
    struct Gadget *gad;
    LONG (*handleIDCMP)(struct gadgetData *gd, struct IntuiMessage *msg);
};
