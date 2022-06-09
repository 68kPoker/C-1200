
/* C-1200 */

/* Gra Magazyn - podstawy silnika */

/* Przygotowanie ca³ej struktury gry polega na zainicjowaniu korzenia, który inicjalizuje swoje 
   komponenty. Komponent jest w zmiennej comp. Ca³a bie¿±ca droga z korzenia do bie¿±cego komponentu 
   jest przechowywana w zmiennej path. Dane tymczasowe we/wy s± przechowywane w zmiennej temp. */

#include "Basics.h"

BOOL init(struct Path *path, struct Game *game)
{
    struct ScreenTempData temp = { 0 };

    path->game = game;
    if (initGraphics((struct GfxPath *)path, &game->gfx, &temp.gfx))
    {
        temp.depth = GetBitMapAttr(game->gfx.bm, BMA_DEPTH);
        if (initScreen((struct ScreenPath *)path, &game->s, &temp))
        {
            freeScreenTemp(&temp);
            return(TRUE);
        }
        freeGraphics(&game->gfx);
    }
    freeScreenTemp(&temp);
    return(FALSE);
}

VOID cleanup(struct Game *game)
{
    freeScreen(&game->s);
    freeGraphics(&game->gfx);
}

BOOL initScreen(struct ScreenPath *path, struct ScreenData *s, struct ScreenTempData *temp)
{    
    UBYTE depth = GetBitMapAttr(temp->depth, BMA_DEPTH);

    path->s = s;
    if (s->bm[0] = AllocBitMap(320, 256, depth, BMF_DISPLAYABLE|BMF_INTERLEAVED, NULL))
    {
        if (s->bm[1] = AllocBitMap(320, 256, depth, BMF_DISPLAYABLE|BMF_INTERLEAVED, NULL))
        {
            if (s->s = OpenScreenTags(NULL,
                SA_BitMap,      s->bm[0],
                SA_Colors32,    temp->gfx.RGB,
                SA_DisplayID,   temp->gfx.modeID,
                SA_Quiet,       TRUE,
                SA_ShowTitle,   FALSE,
                SA_BackFill,    LAYERS_NOBACKFILL,
                SA_Exclusive,   TRUE,
                SA_Title,       temp->title,
                TAG_DONE))
            {
                struct WindowTempData temp = fullWindow(path);
                
                initWindow((struct WindowPath *)path, &s->w, &temp);
            }
            FreeBitMap(s->bm[1]);
        }
        FreeBitMap(s->bm[0]);
    }
    return(FALSE);
}

struct WindowTempData fullWindow(struct ScreenPath *path)
{
    struct WindowTempData temp;

    temp.x = temp.y = 0;
    temp.width = path->s->s->Width;
    temp.height = path->s->s->Height;

    temp.idcmp = IDCMP_RAWKEY|IDCMP_MOUSEBUTTONS|IDCMP_MOUSEMOVE;

    return(temp);
}

BOOL initWindow(struct WindowPath *path, struct WindowData *w, struct WindowTempData *temp)
{
    path->w = w;

    if (w->w = OpenWindowTags(NULL,
        WA_CustomScreen,    path->parent.s->s,
        WA_Left,            temp->x,
        WA_Top,             temp->y,
        WA_Width,           temp->width,
        WA_Height,          temp->height,
        WA_Borderless,      TRUE,
        WA_RMBTrap,         TRUE,
        WA_SimpleRefresh,   TRUE,
        WA_BackFill,        LAYERS_NOBACKFILL,
        WA_IDCMP,           temp->idcmp,
        TAG_DONE))
    {
        return(TRUE);
    }
    return(FALSE);
}

VOID freeWindow(struct WindowData *w)
{
    CloseWindow(w->w);
}

VOID freeScreen(struct ScreenData *s)
{
    CloseScreen(s->s);
}

VOID freeScreenTemp(struct ScreenTempData *temp)
{
    freeGraphicsTemp(&temp->gfx);
}
