
/*
** GameX engine
** Src > Edit
*/

#include <stdio.h>

#include <exec/memory.h>
#include <intuition/intuition.h>

#include <clib/exec_protos.h>

#include "Edit.h"
#include "Bobs.h"

VOID updateSelect(struct selectData *sd, BOOL redraw)
{
    if (redraw)
    {
        WORD row, col;

        for (row = 0; row < sd->height; row++)
        {
            for (col = 0; col < sd->width; col++)
            {
                drawTile(sd->tileGfx, (row * sd->width) + col, sd->wd, sd->gd.gad->LeftEdge + (col << 4), sd->gd.gad->TopEdge + (row << 4));
            }
        }
    }

    if (sd->selected != sd->prevSelected)
    {
        /* Select tile */

        printf("%d selected.\n", sd->selected);

        sd->prevSelected = sd->selected;
    }
}

VOID hitSelect(struct selectData *sd, struct IntuiMessage *msg)
{
    if (msg->Class == IDCMP_GADGETDOWN)
    {
        sd->selected = ((msg->MouseY >> 4) * sd->width) + (msg->MouseX >> 4);

        updateSelect(sd, FALSE);
    }
}

BOOL initSelect(struct selectData *sd, struct gadgetData *prev, WORD left, WORD top, WORD width, WORD height, WORD selected, struct windowData *wd, struct BitMap *gfx)
{
    if (initGadget(&sd->gd, prev, left, top, width << 4, height << 4, GID_SELECT, hitSelect))
    {
        sd->width = width;
        sd->height = height;
        sd->selected = sd->prevSelected = selected;
        sd->wd = wd;
        sd->tileGfx = gfx;

        updateSelect(sd, TRUE);
        return(TRUE);
    }
    return(FALSE);
}

WORD *allocBoard()
{
    WORD *p;

    if (p = AllocVec(WIDTH * HEIGHT * sizeof(*p), MEMF_PUBLIC))
    {
        WORD x, y;

        for (y = 0; y < HEIGHT; y++)
        {
            for (x = 0; x < WIDTH; x++)
            {
                if (x == 0 || x == (WIDTH - 1) || y == 0 || y == (HEIGHT - 1))
                {
                    p[(y * WIDTH) + x] = TID_WALL;
                }
                else
                {
                    p[(y * WIDTH) + x] = TID_FLOOR;
                }
            }
        }
        return(p);
    }
    return(NULL);
}

VOID hitEdit(struct editData *ed, struct IntuiMessage *msg)
{
    WORD x = msg->MouseX >> 4, y = msg->MouseY >> 4; /* Note: Substract gadget pos if needed */

    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
    {
        return;
    }

    if (msg->Class == IDCMP_GADGETDOWN)
    {
        ed->paint = TRUE;
        ed->board[(y * WIDTH) + x] = ed->sd->selected;

        drawTile(ed->tileGfx, ed->sd->selected, ed->wd, ed->gd.gad->LeftEdge + (x << 4), ed->gd.gad->TopEdge + (y << 4));

        ed->wd->activeGad = (struct gadgetData *)ed;
    }
    else if (msg->Class == IDCMP_MOUSEBUTTONS)
    {
        if (msg->Code == IECODE_LBUTTON|IECODE_UP_PREFIX)
        {
            ed->paint = FALSE;
            ed->wd->activeGad = NULL;
        }
    }
    else if (msg->Class == IDCMP_MOUSEMOVE)
    {
        if (ed->paint)
        {
            ed->board[(y * WIDTH) + x] = ed->sd->selected;

            drawTile(ed->tileGfx, ed->sd->selected, ed->wd, ed->gd.gad->LeftEdge + (x << 4), ed->gd.gad->TopEdge + (y << 4));
        }
    }
}

BOOL initEdit(struct editData *ed, struct gadgetData *prev, WORD left, WORD top, struct windowData *wd, struct BitMap *gfx, WORD *board)
{
    if (initGadget(&ed->gd, prev, left, top, WIDTH << 4, HEIGHT << 4, GID_BOARD, hitEdit))
    {
        ed->wd = wd;
        ed->tileGfx = gfx;
        ed->board = board;
        ed->paint = FALSE;
        return(TRUE);
    }
    return(FLASE);
}