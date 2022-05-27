
/*
** GameX engine
** Src > Edit
*/

#include <stdio.h>

#include <exec/memory.h>
#include <intuition/intuition.h>
#include <libraries/iffparse.h>

#include <clib/exec_protos.h>
#include <clib/iffparse_protos.h>

#include "Edit.h"
#include "Bobs.h"
#include "IFF.h"
#include "Windows.h"

#define ID_GAME MAKE_ID('G','A','M','E')
#define ID_MAP  MAKE_ID('M','A','P',' ')
#define ID_BODY MAKE_ID('B','O','D','Y')

UBYTE mapTiles[] = "Back, Wall, Floor, KeyBox, KeyStone, SwitchBox, SwitchStone, Sand, Mud, Water, Fire, Hero, BombBox, Skull, Cherry";

BOOL loadBoard(struct editData *ed, STRPTR name)
{
    struct IFFHandle *iff;

    if (iff = openIFF(name, IFFF_READ))
    {
        if (PropChunk(iff, ID_GAME, ID_MAP) == 0)
        {
            if (StopChunk(iff, ID_GAME, ID_BODY) == 0)
            {
                if (ParseIFF(iff, IFFPARSE_SCAN) == 0)
                {
                    struct ContextNode *cn;

                    if (cn = CurrentChunk(iff))
                    {                    
                        BYTE *buffer;
                        LONG size = cn->cn_Size;
                        if (buffer = AllocVec(size, MEMF_PUBLIC))
                        {
                            if (ReadChunkBytes(iff, ed->board, size) == size)
                            {
                                closeIFF(iff);
                                return(TRUE);
                            }
                        }                    
                    }                    
                }
            }
        }
        closeIFF(iff);
    }
    return(FALSE);
}

BOOL saveBoard(struct editData *ed, STRPTR name)
{
    struct IFFHandle *iff;

    if (iff = openIFF(name, IFFF_WRITE))
    {
        if (PushChunk(iff, ID_GAME, ID_FORM, IFFSIZE_UNKNOWN) == 0)
        {
            if (PushChunk(iff, ID_GAME, ID_MAP, sizeof(mapTiles)) == 0)
            {
                if (WriteChunkBytes(iff, mapTiles, sizeof(mapTiles)) == sizeof(mapTiles))
                {
                    if (PopChunk(iff) == 0)
                    {
                    	LONG size = WIDTH * HEIGHT * sizeof(*ed->board);
                        if (PushChunk(iff, ID_GAME, ID_BODY, size) == 0)
                        {
                            if (WriteChunkBytes(iff, ed->board, size) == size)
                            {
                                if (PopChunk(iff) == 0)
                                {
                                    closeIFF(iff);
                                    return(TRUE);
                                }
                            }
                        }
                    }
                }
            }
        }
        closeIFF(iff);
    }
    return(FALSE);
}

VOID updateSelect(struct selectData *sd, BOOL redraw)
{
    if (redraw)
    {
        WORD row, col;

        for (row = 0; row < sd->height; row++)
        {
            for (col = 0; col < sd->width; col++)
            {
                drawTile(sd->tileGfx, (row * sd->width) + col, sd->wd->w->RPort, sd->gd.gad->LeftEdge + (col << 4), sd->gd.gad->TopEdge + (row << 4));
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
    if (initGadget(&sd->gd, prev, left, top, width << 4, height << 4, GID_SELECT, (VOID (*handle)(struct gadgetData *gd, struct IntuiMessage *msg))hitSelect))
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
    WORD x = (msg->MouseX - ed->gd.gad->LeftEdge) >> 4, y = (msg->MouseY - ed->gd.gad->TopEdge) >> 4; /* Note: Substract gadget pos if needed */

    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
    {
        return;
    }

    if (msg->Class == IDCMP_GADGETDOWN)
    {
        ed->paint = TRUE;
        ed->board[(y * WIDTH) + x] = ed->sd->selected;

        drawTile(ed->tileGfx, ed->sd->selected, ed->wd->w->RPort, ed->gd.gad->LeftEdge + (x << 4), ed->gd.gad->TopEdge + (y << 4));
        drawFrame(ed->tileGfx, ed->wd->w->RPort, ed->gd.gad->LeftEdge + (x << 4), ed->gd.gad->TopEdge + (y << 4));

        ed->wd->activeGad = (struct gadgetData *)ed;
    }
    else if (msg->Class == IDCMP_MOUSEBUTTONS)
    {
        if (msg->Code == (IECODE_LBUTTON|IECODE_UP_PREFIX))
        {
            ed->paint = FALSE;
            ed->wd->activeGad = NULL;
        }
    }
    else if (msg->Class == IDCMP_MOUSEMOVE)
    {
        if (ed->cursX != x || ed->cursY != y)
        {
            if (ed->paint)
            {
                ed->board[(y * WIDTH) + x] = ed->sd->selected;

                drawTile(ed->tileGfx, ed->sd->selected, ed->wd->w->RPort, ed->gd.gad->LeftEdge + (x << 4), ed->gd.gad->TopEdge + (y << 4));                
            }        
            drawTile(ed->tileGfx, ed->board[(ed->cursY * WIDTH) + ed->cursX], ed->wd->w->RPort, ed->gd.gad->LeftEdge + (ed->cursX << 4), ed->gd.gad->TopEdge + (ed->cursY << 4));
            ed->cursX = x;
            ed->cursY = y;                        
            drawFrame(ed->tileGfx, ed->wd->w->RPort, ed->gd.gad->LeftEdge + (x << 4), ed->gd.gad->TopEdge + (y << 4));
        }
    }
}

BOOL initEdit(struct editData *ed, struct gadgetData *prev, WORD left, WORD top, struct windowData *wd, struct BitMap *gfx, WORD *board)
{
    if (initGadget(&ed->gd, prev, left, top, WIDTH << 4, HEIGHT << 4, GID_BOARD, (VOID (*handle)(struct gadgetData *gd, struct IntuiMessage *msg))hitEdit))
    {
        ed->wd = wd;
        ed->tileGfx = gfx;
        ed->board = board;
        ed->paint = FALSE;        
        ed->cursX = ed->cursY = 0;
        return(TRUE);
    }
    return(FALSE);
}