
#include <stdio.h>
#include "debug.h"

#include <dos/dos.h>
#include <intuition/intuition.h>
#include <libraries/iffparse.h>

#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/alib_protos.h>

#include "Libs.h"
#include "IFF.h"
#include "ILBM.h"
#include "Screen.h"
#include "Joy.h"
#include "Bobs.h"
#include "Windows.h"
#include "Edit.h"
#include "Loop.h"
#include "Blitter.h"
#include "Board.h"

#define MENU_LEFT 0
#define MENU_TOP  0
#define MENU_WIDTH 64
#define MENU_HEIGHT 16

VOID prepBitMap(board *board, struct RastPort *rp, struct screenData *sd);

struct boardWindowData
{
    struct windowData wd;
    struct editData ed; /* Board-editing gadget */
    struct selectData seld; /* Tile-selection gadget */
    struct menuGadgetData menud; /* Menu button */
};

struct TextAttr ta = 
{
    "centurion.font", 9, FS_NORMAL, FPF_DISKFONT|FPF_DESIGNED
};

struct Rectangle dclip =
{
    0, 0, 319, 255
};

/* Distribution of graphics frames */
WORD gfxCount[T_COUNT];

/* Do the setup (graphics, screen, joystick, window) */

BOOL setup(STRPTR name, struct screenData *sd, ULONG idcmp)
{
    struct IFFHandle *iff;
    struct windowData *wd = sd->wd;
    const WORD width = 320, height = 256;

    /* Read graphics */
    if (iff = openIFF(name, IFFF_READ))
    {
        if (scanILBM(iff))
        {
            ULONG *pal;
            if (pal = getPal(iff))
            {
                struct BitMap *gfx;
                if (gfx = unpackBitMap(iff, &sd->mask))
                {
                    struct BitMap *bm[2];
                    WORD depth = gfx->Depth;
                    
                    sd->gfx = gfx;

                    if (bm[0] = AllocBitMap(width, height, depth, BMF_DISPLAYABLE|BMF_INTERLEAVED, NULL))
                    {
                        if (bm[1] = AllocBitMap(width, height, depth, BMF_DISPLAYABLE|BMF_INTERLEAVED, NULL))
                        {                
                            struct RastPort rp;
                            
                            InitRastPort(&rp);
                            rp.BitMap = bm[0];
                            
                            /* Draw initial screen contents before opening */
                            prepBitMap(NULL, &rp, sd);                         

                            if (openScreen(sd, "Warehouse", PAL_MONITOR_ID|LORES_KEY, &dclip, bm, pal, &ta))
                            {                            
                                if (addCop(sd, "Warehouse", 0, 0, NULL))
                                {
                                    if (addDBuf(sd))
                                    {                                    
                                        if (openWindow(wd, sd, 
                                            WA_Left,        0, 
                                            WA_Top,         0,
                                            WA_Width,       sd->s->Width,
                                            WA_Height,      sd->s->Height,
                                            WA_Activate,    TRUE,
                                            WA_IDCMP,       idcmp,
                                            TAG_DONE))
                                        {                                                                                        
                                            if (sd->joyIO = openJoy(&sd->joyIE))
                                            {
                                                NewList(&sd->bobs);
                                                FreeVec(pal);
                                                closeIFF(iff);
                                                return(TRUE);
                                            }
                                            closeWindow(wd);
                                        }
                                        remDBuf(sd);
                                    }
                                    remCop(sd);
                                }
                                closeScreen(sd);
                            }
                            FreeBitMap(bm[1]);
                        }
                        FreeBitMap(bm[0]);
                    }
                    FreeBitMap(gfx);
                    if (sd->mask)
                    {
                        FreeVec(sd->mask);
                    }   
                }
                FreeVec(pal);
            }
        }
        closeIFF(iff);
    }
    return(FALSE);
}

VOID cleanup(struct screenData *sd)
{
    struct windowData *wd = sd->wd;

    closeJoy(sd->joyIO);
    closeWindow(wd);
    remDBuf(sd);
    remCop(sd);
    closeScreen(sd);
    FreeBitMap(sd->bm[1]);
    FreeBitMap(sd->bm[0]);
    FreeBitMap(sd->gfx);
    if (sd->mask)
    {
        FreeVec(sd->mask);
    }   
}

/* Prepare main GUI */

BOOL prepGUI(struct screenData *sd, struct boardWindowData *bwd, board *board)
{
    bwd->ed.sd = &bwd->seld; /* Link gadgets */

    if (initSelect(&bwd->seld, NULL, 64, 0, T_COUNT, 1, 1, &bwd->wd, sd))
    {
        if (initEdit(&bwd->ed, &bwd->seld.gd, 0, 16, &bwd->wd, sd, board))
        {
            if (initGadget(&bwd->menud.gd, &bwd->ed.gd, MENU_LEFT, MENU_TOP, MENU_WIDTH, MENU_HEIGHT, GID_MENU, (APTR)hitMenu))
            {
                bwd->menud.ed = &bwd->ed;
                
                AddGList(bwd->wd.w, bwd->seld.gd.gad, -1, -1, NULL);
                return(TRUE);
            }
            freeGadget((struct gadgetData *)&bwd->ed);
        }    
        freeGadget((struct gadgetData *)&bwd->seld);
    }    
    return(FALSE);
}

VOID cleanGUI(struct boardWindowData *bwd)
{
    RemoveGList(bwd->wd.w, bwd->seld.gd.gad, -1);
    freeGadget(&bwd->menud.gd);
    freeGadget((struct gadgetData *)&bwd->ed);
    freeGadget((struct gadgetData *)&bwd->seld);
}

VOID prepBitMap(board *board, struct RastPort *rp, struct screenData *sd)
{
    WORD x, y;

    for (y = 1; y < B_HEIGHT; y++)
    {
        for (x = 0; x < B_WIDTH; x++)
        {
            tile *tile;
            
            if (board)
            {
                tile = ((struct sTile *)board->board + (y * B_WIDTH) + x);
            }
            else
            {
                tile = tileTypes + T_FLOOR; /* Default floor */
            }   
            drawTile(sd, tile, rp, x << 4, y << 4, FALSE);
        }               
    }
    bltBitMapRastPort(sd->gfx, 240, 0, rp, 0, 0, 64, 16, 0xc0);
    bltBitMapRastPort(sd->gfx, 304, 0, rp, 304, 0, 16, 16, 0xc0);
    bltBitMapRastPort(sd->gfx, 0, 0, rp, 64, 0, T_COUNT << 4, 16, 0xc0);
}

int main(int argc, char **argv)
{
    if (openLibs())
    {
        static struct screenData sd;
        static struct boardWindowData bwd; /* Custom window */

        sd.wd = &bwd.wd;

        constructGfx(gfxCount, T_COUNT);

        if (setup("Data/Graphics.iff", &sd, IDCMP_RAWKEY|IDCMP_GADGETDOWN|IDCMP_MOUSEMOVE|IDCMP_MOUSEBUTTONS|IDCMP_REFRESHWINDOW))
        {
            static board board;
            /* Allocate space for board */

            constructBoard(&board);
        
            /* Setup hero Bob */
            constructBob(sd.bob + TID_HERO - 1, sd.gfx, gfxCount[T_HERO] << 4, 0, 10 << 4, 8 << 4);
            
            /* sd.bob[0].animate = animateHero; */
            
            sd.bob[1].update[0] = sd.bob[1].update[1] = FALSE;
            
            AddTail(&sd.bobs, &sd.bob[0].node);
            AddTail(&sd.bobs, &sd.bob[1].node);
            
            if (prepGUI(&sd, &bwd, &board))
            {
                eventLoop(&sd, &bwd.wd, &board);
                cleanGUI(&bwd);
            }        
            cleanup(&sd);
        }
        closeLibs();
    }
    else
        printf("Couldn't open required libraries V39!\n");
    return(RETURN_OK);
}
