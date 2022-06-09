
/*
** C-1200 engine
** Src > Bobs
**
** Blitter low-level drawing functions
*/

#ifndef BLITTER_H
#define BLITTER_H

#include <exec/types.h>
#include <graphics/gfx.h>

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

/* This is a family of fast, aligned drawing operations that work on any layered or non-layered RastPort 
 * with regions installed. */

VOID clipBlit(struct RastPort *srp, WORD sx, WORD sy, struct RastPort *drp, WORD dx, WORD dy, WORD width, WORD height, UBYTE minterm);
VOID bltBitMapRastPort(struct BitMap *sbm, WORD sx, WORD sy, struct RastPort *drp, WORD dx, WORD dy, WORD width, WORD height, UBYTE minterm);
VOID bltBitMap(struct BitMap *sbm, WORD sx, WORD sy, struct BitMap *dbm, WORD dx, WORD dy, WORD width, WORD height, UBYTE minterm);

/* This renders through mask, used to draw Bobs */
VOID bltMaskBitMapRastPort(struct BitMap *src, WORD sx, WORD sy, struct RastPort *rp, WORD dx, WORD dy, WORD width, WORD height, UBYTE minterm, PLANEPTR mask);

#endif /* BLITTER_H */
