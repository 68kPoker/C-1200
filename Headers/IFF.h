
/*
** C-1200 engine
** Src > Init > IFF
*/

#ifndef IFF_H
#define IFF_H

#include <exec/types.h>

struct IFFHandle *openIFF(STRPTR name, LONG mode);
VOID closeIFF(struct IFFHandle *iff);
BYTE *findPropData(struct IFFHandle *iff, ULONG type, ULONG ID);
BYTE *getChunkBuffer(struct IFFHandle *iff, LONG *pSize);

#endif /* IFF_H */
