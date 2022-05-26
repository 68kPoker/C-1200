
/*
** GameX engine
** Src > Edit
*/

#include <stdio.h>

#include <intuition/intuition.h>

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
