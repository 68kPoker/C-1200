
/* C-1200 */

#include <exec/types.h>

BOOL initGraphics(struct GfxPath *path, struct GfxData *gfx, struct GfxTempData *temp);
VOID freeGraphics(struct GfxData *gfx);
VOID freeGraphicsTemp(struct GfxTempData *temp);
