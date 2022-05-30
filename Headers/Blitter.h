
/*
** GameX engine
** Src > Bobs
*/

#ifndef BLITTER_H
#define BLITTER_H

#include <exec/types.h>

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

VOID clipBlit(struct RastPort *srp, WORD sx, WORD sy, struct RastPort *drp, WORD dx, WORD dy, WORD width, WORD height, UBYTE minterm);
VOID bltBitMapRastPort(struct BitMap *sbm, WORD sx, WORD sy, struct RastPort *drp, WORD dx, WORD dy, WORD width, WORD height, UBYTE minterm);
VOID bltBitMap(struct BitMap *sbm, WORD sx, WORD sy, struct BitMap *dbm, WORD dx, WORD dy, WORD width, WORD height, UBYTE minterm);

/* Mask is in source BitMap last plane */
VOID bltMaskBitMapRastPort(struct BitMap *src, WORD sx, WORD sy, struct RastPort *rp, WORD dx, WORD dy, WORD width, WORD height, UBYTE minterm);

#endif /* BLITTER_H */
