
/* C-1200 */

#include <exec/types.h>
#include <graphics/gfx.h>

struct WindowData
{
    struct Window *w;
};

struct WindowTempData
{
    WORD x, y, width, height;
    ULONG idcmp;
};

struct ScreenData
{
    struct BitMap *bm[2];
    struct Screen *s;
    struct WindowData w;
};

struct GfxData
{
    struct BitMap *bm;
    PLANEPTR mask;
};

struct GfxTempData
{    
    ULONG *RGB;
    ULONG modeID;
    STRPTR name;
};

struct ScreenTempData
{
    struct GfxTempData gfx; /* Nested temporary data */
    WORD depth;
    STRPTR title;
};

struct Game
{
    struct GfxData gfx; /* Graphics and palette */
    struct ScreenData s; /* Screen with UserData */
};

struct Path
{
    struct Game *game;
};

struct ScreenPath 
{
    struct Path parent;
    struct ScreenData *s;
};

struct WindowPath
{
    struct ScreenPath parent;
    struct WindowData *w;    
};

struct GfxPath
{
    struct Path parent;
    struct GfxData *gfx;
};

BOOL init(struct Path *path, struct Game *game);
VOID cleanup(struct Game *game);

BOOL initScreen(struct ScreenPath *path, struct ScreenData *s, struct ScreenTempData *temp);
VOID freeScreen(struct ScreenData *s);
VOID freeScreenTemp(struct ScreenTempData *temp);

struct WindowTempData fullWindow(struct ScreenPath *path);

BOOL initWindow(struct WindowPath *path, struct WindowData *w, struct WindowTempData *temp);
