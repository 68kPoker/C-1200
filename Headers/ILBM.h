
/*
** GameX engine
** Src > Init > IFF
*/

#ifndef ILBM_H
#define ILBM_H

#include <exec/types.h>

#define RGB(c) ((c)|((c)<<8)|((c)<<16)|((c)<<24))
#define RowBytes(w) ((((w) + 15) >> 4) << 1)

BOOL scanILBM(struct IFFHandle *iff);
ULONG *getPal(struct IFFHandle *iff);
struct BitMap *unpackBitMap(struct IFFHandle *iff);

#endif /* ILBM_H */
