
/*
** GameX engine
** Src > Bobs
*/

#include <intuition/intuition.h>

#include <clib/graphics_protos.h>
#include <clib/exec_protos.h>

#include "Bobs.h"
#include "Blitter.h"

#define MAX_OBJECTS 20

#define WHITE 2

VOID drawTile(struct BitMap *bm, WORD tile, struct RastPort *rp, WORD xpos, WORD ypos)
{
    bltBitMapRastPort(bm, (tile % TILES) << 4, (tile / TILES) << 4, rp, xpos, ypos, 16, 16, 0xc0);
}

VOID drawFrame(struct BitMap *bm, struct RastPort *rp, WORD x, WORD y)
{
    SetAPen(rp, WHITE);
    Move(rp, x, y);
    Draw(rp, x + 15, y);
    Draw(rp, x + 15, y + 15);
    Draw(rp, x, y + 15);
    Draw(rp, x, y + 1);
}

VOID initBob(struct List *list, struct bobData *bd, struct BitMap *gfx, WORD gfxX, WORD gfxY, WORD posX, WORD posY)
{
    AddTail(list, &bd->node);

    bd->gfx = gfx;
    bd->gfxX = gfxX;
    bd->gfxY = gfxY;
    bd->posX = posX;
    bd->posY = posY;

    bd->update[0] = bd->update[1] = TRUE; /* This Bob must be drawn */
    bd->active = TRUE; /* This Bob is active */
}

/* Clear background under Bobs (using tile graphics) */
VOID clearBG(struct List *list, struct RastPort *rp, WORD frame, struct BitMap *tileGfx, WORD *board)
{
    struct Node *node;
    WORD offsets[MAX_OBJECTS * 2];
    WORD i, n = 0;

    for (node = list->lh_Head; node->ln_Succ != NULL; node = node->ln_Succ)
    {
        struct bobData *bd = (struct bobData *)node;

        if (bd->update[frame])
        {
            WORD x = bd->posX >> 4;
            WORD y = bd->posY >> 4;
            WORD offset = (y * WIDTH) + x;

            WORD *tile = &board[offset];

            if (!(*tile & 0x8000))
            {                
                drawTile(tileGfx, *tile, rp, x << 4, y << 4);
                *tile |= 0x8000; /* Flag this tile as drawn */
                offsets[n++] = offset;
            }

            x = (bd->posX + 15) >> 4;
            y = (bd->posY + 15) >> 4;
            offset = (y * WIDTH) + x;

            *tile = &board[offset];

            if (!(*tile & 0x8000))
            {
                drawTile(tileGfx, *tile, rp, x << 4, y << 4);
                *tile |= 0x8000;
                offsets[n++] = offset;
            }
        }
    }

    /* Unflag floors */
    for (i = 0; i < n; i++)
    {
        board[offsets[i]] &= 0x7fff;
    }
}

/* Draw Bob if required */
VOID drawBob(struct bobData *bd, struct RastPort *rp, WORD frame)
{
    if (!bd->update[frame])
    {
        /* Bob doesn't require update in this frame */
        return;
    }

    /* We assume, background is cleaned up */

    /* Mask is included in source BitMap */
    bltMaskBitMapRastPort(bd->gfx, bd->gfxX, bd->gfxY, rp, bd->posX, bd->posY, bd->width, bd->height, ABC|ABNC|ANBC);
}

/* Draw Bob list */
VOID drawBobs(struct List *list, struct RastPort *rp, WORD frame)
{
    struct Node *node;

    for (node = list->lh_Head; node->ln_Succ != NULL; node = node->ln_Succ)
    {
        drawBob((struct bobData *)node, rp, frame);
    }
}
