
#include <exec/types.h>

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

void clipBlit(struct RastPort *srp, WORD sx, WORD sy, struct RastPort *drp, WORD dx, WORD dy, WORD width, WORD height, UBYTE minterm);
void bltBitMapRastPort(struct BitMap *sbm, WORD sx, WORD sy, struct RastPort *drp, WORD dx, WORD dy, WORD width, WORD height, UBYTE minterm);
