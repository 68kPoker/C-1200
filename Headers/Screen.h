
/* C-1200 Engine Source Code (in C) */

#ifndef SCREEN_H
#define SCREEN_H

#include <exec/types.h>

#define SCREEN_BITMAP_WIDTH  320
#define SCREEN_BITMAP_HEIGHT 256

BOOL openScreen(struct screenData *sd, STRPTR title, ULONG modeID, struct Rectangle *dclip, ULONG *pal, struct TextAttr *ta);
VOID closeScreen(struct screenData *sd);
BOOL addCop(struct screenData *sd, STRPTR name, BYTE pri, WORD coplen, VOID (*addCustomCop)(struct screenData *sd, struct UCopList *ucl));
VOID remCop(struct screenData *sd);
BOOL addDBuf(struct screenData *sd);
VOID remDBuf(struct screenData *sd);
VOID safeToDraw(struct screenData *sd);
VOID swapBuf(struct screenData *sd);

#endif /* SCREEN_H */
