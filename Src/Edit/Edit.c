
/*
** C-1200 engine
** Src > Edit
**
** Game-editing functions
*/

#include <stdio.h>
#include <string.h>

#include <exec/memory.h>
#include <intuition/intuition.h>
#include <libraries/iffparse.h>

#include <clib/exec_protos.h>
#include <clib/iffparse_protos.h>
#include <clib/graphics_protos.h>

#include "Edit.h"
#include "Bobs.h"
#include "IFF.h"
#include "Screen.h"
#include "Windows.h"
#include "Blitter.h"

#include "debug.h"

#define MAX_LEVELS 30

#define WHITE 2
#define ESC_KEY 0x45

#define ID_GAME MAKE_ID('G','A','M','E')
#define ID_MAP  MAKE_ID('M','A','P',' ')
#define ID_BODY MAKE_ID('B','O','D','Y')

VOID prepBitMap(board *board, struct RastPort *rp, struct screenData *sd);

UBYTE mapTiles[] = "Warehouse board file v1.7";

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
                        LONG size = cn->cn_Size;
                        
                        if (ReadChunkBytes(iff, ed->board, size) == size)
                        {
                            closeIFF(iff);
                            return(TRUE);
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
                        LONG size = B_WIDTH * B_HEIGHT * sizeof(*ed->board);
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
    WORD row, col;
    if (redraw)
    {        
        for (row = 0; row < sd->height; row++)
        {
            for (col = 0; col < sd->width; col++)
            {
                drawTile(sd->screen, tileTypes + (row * sd->width) + col, sd->wd->w->RPort, sd->gd.gad->LeftEdge + (col << 4), sd->gd.gad->TopEdge + (row << 4), FALSE);
            }
        }
        col = sd->selected % sd->width;
        row = sd->selected / sd->width;
        drawFrame(sd->wd->w->RPort, sd->gd.gad->LeftEdge + (col << 4), sd->gd.gad->TopEdge + (row << 4));
    }

    if (sd->selected != sd->prevSelected)
    {
        /* Select tile */
        
        col = sd->prevSelected % sd->width;
        row = sd->prevSelected / sd->width;
        drawTile(sd->screen, tileTypes + sd->prevSelected, sd->wd->w->RPort, sd->gd.gad->LeftEdge + (col << 4), sd->gd.gad->TopEdge + (row << 4), FALSE);

        col = sd->selected % sd->width;
        row = sd->selected / sd->width;
        drawFrame(sd->wd->w->RPort, sd->gd.gad->LeftEdge + (col << 4), sd->gd.gad->TopEdge + (row << 4));

        sd->prevSelected = sd->selected;
    }
}

VOID hitSelect(struct selectData *sd, struct IntuiMessage *msg)
{
    if (msg->Class == IDCMP_GADGETDOWN)
    {
        sd->selected = (((msg->MouseY - sd->gd.gad->TopEdge) >> 4) * sd->width) + ((msg->MouseX - sd->gd.gad->LeftEdge) >> 4);

        updateSelect(sd, FALSE);
    }
}

LONG hitButton(struct gadgetData *gd, struct IntuiMessage *msg)
{
    WORD action = AID_NONE;
    struct menuData *md = (struct menuData *)msg->IDCMPWindow->UserData;

    if (msg->Class == IDCMP_GADGETDOWN)
    {        
        switch (gd->gad->GadgetID)
        {
            case GID_LOAD:
                md->action = AID_LOAD;
                md->wd.done = TRUE;
                break;
            case GID_CANCEL:
                md->action = AID_CANCEL;
                md->wd.done = TRUE;
                break;
            case GID_SAVE:
                md->action = AID_SAVE;
                md->wd.done = TRUE;
                break;
            case GID_PREV:
                if (md->level > 0)
                {
                    md->action = AID_PREV;
                    md->level--;
                    /* Decrease and redraw level number */
                }    
                break;
            case GID_NEXT:
                if (md->level < MAX_LEVELS)
                {
                    md->action = AID_NEXT;
                    md->level++;
                    /* Increase and redraw level number */
                }    
                break;
            default:
        }
        action = md->action;
    }
    return(action);
}

VOID drawNumber(struct RastPort *rp, WORD number)
{
    UBYTE text[6];
    WORD posx = 132, posy = 20;

    sprintf(text, "%4d ", number);

    SetAPen(rp, WHITE);
    SetBPen(rp, 5);
    SetDrMd(rp, JAM2);
    Move(rp, posx, posy + rp->Font->tf_Baseline);
    Text(rp, text, strlen(text));
}

VOID drawMenu(struct menuData *md)
{
    struct screenData *sd = (struct screenData *)md->wd.w->WScreen->UserData;    
    WORD menuX = 0, menuY = 16;
    
    bltBitMapRastPort(sd->gfx, menuX, menuY, md->wd.w->RPort, 0, 0, md->wd.w->Width, md->wd.w->Height, 0xc0);
}

LONG handleMenu(struct menuData *md)
{
    struct IntuiMessage *msg;
    LONG action = AID_NONE;

    while (!md->wd.done)
    {
        WaitPort(md->wd.w->UserPort);

        while ((!md->wd.done) && (msg = (struct IntuiMessage *)GetMsg(md->wd.w->UserPort)))
        {
            if (msg->Class == IDCMP_GADGETDOWN)
            {
                struct Gadget *gad = (struct Gadget *)msg->IAddress;
                struct gadgetData *gd = (struct gadgetData *)gad->UserData;

                if (gd->handleIDCMP)
                {
                    action = gd->handleIDCMP(gd, msg);    

                    switch (action)
                    {
                        case AID_PREV:                                                
                        case AID_NEXT:
                            drawNumber(md->wd.w->RPort, md->level);
                            break;                    
                    }              
                }
            }
            else if (msg->Class == IDCMP_RAWKEY)
            {
                if (msg->Code == ESC_KEY)
                {
                    md->wd.done = TRUE;
                }
            }
            ReplyMsg((struct Message *)msg);
        }
    }
    return(action);
}

VOID hitMenu(struct menuGadgetData *mgd, struct IntuiMessage *msg)
{
    struct screenData *sd = (struct screenData *)msg->IDCMPWindow->WScreen->UserData;
    struct gadgetData *gd = &mgd->gd;

    if (msg->Class == IDCMP_GADGETDOWN)
    {
        struct Requester req;
        struct windowData *wd = (struct windowData *)msg->IDCMPWindow->UserData;
        static struct menuData rwd;
               
        /* Open menu */
        InitRequester(&req);
        Request(&req, wd->w);

        if (openWindow((struct windowData *)&rwd, sd, 
            WA_Left,        0,
            WA_Top,         16,
            WA_Width,       160,
            WA_Height,      128,
            WA_Activate,    TRUE,
            WA_IDCMP,       IDCMP_GADGETDOWN|IDCMP_RAWKEY,
            TAG_DONE))
        {
            static struct gadgetData loadGad, cancelGad, saveGad, prevGad, nextGad;

            if (initGadget(&loadGad, NULL, 0, 16, 64, 16, GID_LOAD, (APTR)hitButton))
            {
                if (initGadget(&cancelGad, &loadGad, 64, 16, 64, 16, GID_CANCEL, (APTR)hitButton))
                {
                    if (initGadget(&prevGad, &cancelGad, 0, 32, 64, 16, GID_PREV, (APTR)hitButton))
                    {
                        if (initGadget(&nextGad, &prevGad, 64, 32, 64, 16, GID_NEXT, (APTR)hitButton))
                        {
                            if (initGadget(&saveGad, &nextGad, 0, 48, 64, 16, GID_SAVE, (APTR)hitButton))
                            {
                                WORD action;
                                UBYTE name[10];
                                WORD level = rwd.level;
                                
                                drawMenu(&rwd);
                                drawNumber(rwd.wd.w->RPort, rwd.level);
                                AddGList(rwd.wd.w, loadGad.gad, -1, -1, NULL);                                
                                
                                action = handleMenu(&rwd);
                                
                                if (action == AID_CANCEL || action == AID_SAVE)
                                {
                                    rwd.level = level;
                                }   
                                
                                if (action == AID_LOAD)
                                {
                                    sprintf(name, "%02d.lev", rwd.level);
                                    if (loadBoard(mgd->ed, name))
                                    {
                                        prepBitMap(mgd->ed->board, wd->w->RPort, sd);
                                    }
                                }
                                else if (action == AID_SAVE)
                                {
                                    sprintf(name, "%02d.lev", rwd.level);
                                    saveBoard(mgd->ed, name);
                                }   
                                
                                RemoveGList(rwd.wd.w, loadGad.gad, -1);
                                freeGadget(&saveGad);
                            }
                            freeGadget(&nextGad);
                        }
                        freeGadget(&prevGad);
                    }
                    freeGadget(&cancelGad);
                }
                freeGadget(&loadGad);
            }
            closeWindow(&rwd.wd);
        }
        EndRequest(&req, msg->IDCMPWindow);
    }
}

BOOL initSelect(struct selectData *sd, struct gadgetData *prev, WORD left, WORD top, WORD width, WORD height, WORD selected, struct windowData *wd, struct screenData *screen)
{
    if (initGadget(&sd->gd, prev, left, top, width << 4, height << 4, GID_SELECT, (VOID (*handle)(struct gadgetData *gd, struct IntuiMessage *msg))hitSelect))
    {
        sd->width = width;
        sd->height = height;
        sd->selected = sd->prevSelected = selected;
        sd->wd = wd;
        sd->screen = screen;

        updateSelect(sd, TRUE);
        return(TRUE);
    }
    return(FALSE);
}

VOID hitEdit(struct editData *ed, struct IntuiMessage *msg)
{
    WORD x = (msg->MouseX - ed->gd.gad->LeftEdge) >> 4, y = (msg->MouseY - ed->gd.gad->TopEdge) >> 4; /* Note: Substract gadget pos if needed */

    if (msg->Class == IDCMP_GADGETDOWN)
    {
        ed->paint = TRUE;
        
        tile *tile = (struct sTile *)ed->board + ((y + 1) * B_WIDTH) + x;
        WORD select = ed->sd->selected;
        
        constructTile(tile, tileTypes[select].floor, tileTypes[select].type);

        drawTile(ed->screen, tileTypes + ed->sd->selected, ed->wd->w->RPort, ed->gd.gad->LeftEdge + (x << 4), ed->gd.gad->TopEdge + (y << 4), FALSE);
        drawFrame(ed->wd->w->RPort, ed->gd.gad->LeftEdge + (x << 4), ed->gd.gad->TopEdge + (y << 4));

        ed->cursX = x;
        ed->cursY = y;        
        ed->wd->activeGad = (struct gadgetData *)ed;
    }
    else if (msg->Class == IDCMP_MOUSEBUTTONS)
    {
        if (msg->Code == (IECODE_LBUTTON|IECODE_UP_PREFIX))
        {
            ed->paint = FALSE;
            ed->wd->activeGad = NULL;
            
            if (ed->cursX >= 0 && ed->cursX < B_WIDTH && ed->cursY >= 0 && ed->cursY < B_HEIGHT)
            {
                drawTile(ed->screen, (struct sTile *)ed->board->board + (ed->cursY + 1) * B_WIDTH + ed->cursX, ed->wd->w->RPort, ed->gd.gad->LeftEdge + (ed->cursX << 4), ed->gd.gad->TopEdge + (ed->cursY << 4), FALSE);
            }    
        }
    }
    else if (msg->Class == IDCMP_MOUSEMOVE)
    {
        if (x >= 0 && x < B_WIDTH && y >= 0 && y < B_HEIGHT)
        {
            if (ed->cursX != x || ed->cursY != y)
            {
                if (ed->paint)
                {
                    *((struct sTile *)ed->board + ((y + 1) * B_WIDTH) + x) = tileTypes[ed->sd->selected];

                    drawTile(ed->screen, tileTypes + ed->sd->selected, ed->wd->w->RPort, ed->gd.gad->LeftEdge + (x << 4), ed->gd.gad->TopEdge + (y << 4), FALSE);                
                }        
                drawTile(ed->screen, (struct sTile *)ed->board->board + (ed->cursY + 1) * B_WIDTH + ed->cursX, ed->wd->w->RPort, ed->gd.gad->LeftEdge + (ed->cursX << 4), ed->gd.gad->TopEdge + (ed->cursY << 4), FALSE);
                ed->cursX = x;
                ed->cursY = y;                        
                drawFrame(ed->wd->w->RPort, ed->gd.gad->LeftEdge + (x << 4), ed->gd.gad->TopEdge + (y << 4));
            }    
        }
    }
}

BOOL initEdit(struct editData *ed, struct gadgetData *prev, WORD left, WORD top, struct windowData *wd, struct screenData *screen, board *board)
{
    if (initGadget(&ed->gd, prev, left, top, B_WIDTH << 4, B_HEIGHT << 4, GID_BOARD, (VOID (*handle)(struct gadgetData *gd, struct IntuiMessage *msg))hitEdit))
    {
        ed->wd = wd;
        ed->screen = screen;
        ed->board = board;
        ed->paint = FALSE;        
        ed->cursX = ed->cursY = 0;
        return(TRUE);
    }
    return(FALSE);
}
