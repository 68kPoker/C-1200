
#include <exec/types.h>

struct windowData
{
    struct Window *w;
    struct gadgetData *activeGad;
    BOOL done;
};
