
#include <stdio.h>

#include <dos/dos.h>
#include <intuition/intuition.h>
#include <libraries/iffparse.h>

#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>

#include "Libs.h"
#include "IFF.h"
#include "ILBM.h"
#include "Screen.h"
#include "Joy.h"
#include "Bobs.h"
#include "Windows.h"
#include "Edit.h"
#include "Loop.h"

#define MENU_LEFT 288
#define MENU_TOP  0
#define MENU_WIDTH 32
#define MENU_HEIGHT 16

struct boardWindowData
{
    struct windowData wd;
    struct editData ed; /* Board-editing gadget */
    struct selectData seld; /* Tile-selection gadget */
    struct gadgetData menud; /* Menu button */
};

struct TextAttr ta = 
{
    "centurion.font", 9, FS_NORMAL, FPF_DISKFONT|FPF_DESIGNED
};

struct Rectangle dclip =
{
    0, 0, 319, 255
};

/* Do the setup (graphics, screen, joystick, window) */

BOOL setup(STRPTR name, struct screenData *sd, ULONG idcmp)
{
    struct IFFHandle *iff;
    struct windowData *wd = &sd->wd;
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
                if (gfx = unpackBitMap(iff))
                {
                    struct BitMap *bm[2];

                    if (bm[0] = AllocBitMap(width, height, gfx->Depth, BMF_DISPLAYABLE|BMF_INTERLEAVED, NULL))
                    {
                        if (bm[1] = AllocBitMap(width, height, gfx->Depth, BMF_DISPLAYABLE|BMF_INTERLEAVED, NULL))
                        {                
                            /* Draw initial screen contents before opening */
                            prepBitMap(bm[0], gfx);                         

                            if (openScreen(sd, "Warehouse", PAL_MONITOR_ID|LORES_KEY, &dclip, bm, pal, &ta))
                            {                            
                                sd->gfx = gfx;
                                if (addCop(sd, "Warehouse", 0, 0, NULL))
                                {
                                    if (addDBuf(sd))
                                    {                                    
                                        if (openWindow(wd, sd, 
                                            WA_Left, 		0, 
                                            WA_Top, 		0,
                                            WA_Width, 		sd->s->Width,
                                            WA_Height, 		sd->s->Height,
                                            WA_Activate,	TRUE,
                                            WA_IDCMP,       idcmp,
                                            TAG_DONE))
                                        {                                                                                        
                                            if (sd->joyIO = openJoy(&sd->joyIE))
                                            {
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
    struct windowData *wd = &sd->wd;

    closeJoy(sd->joyIO);
    closeWindow(wd);
    remDBuf(sd);
    remCop(sd);
    closeScreen(sd);
    FreeBitMap(sd->bm[1]);
    FreeBitMap(sd->bm[0]);
    FreeBitMap(sd->gfx);
}

/* Prepare main GUI */

BOOL prepGUI(struct screenData *sd, struct boardWindowData *bwd, WORD *board)
{
    bwd->ed.sd = &bwd->seld; /* Link gadgets */

    if (initSelect(&bwd->seld, NULL, 0, 0, TID_COUNT, 1, 1, &bwd->wd, sd->gfx))
    {
        if (initEdit(&bwd->ed, &bwd->seld.gd, 0, 16, &bwd->wd, sd->gfx, board))
        {
            if (initGadget(&bwd->menud, &bwd->ed.gd, MENU_LEFT, MENU_TOP, MENU_WIDTH, MENU_HEIGHT, GID_MENU, hitMenu))
            {
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
    freeGadget(&bwd->menud);
    freeGadget((struct gadgetData *)&bwd->ed);
    freeGadget((struct gadgetData *)&bwd->seld);
}

VOID prepBitMap(struct BitMap *bm, struct BitMap *gfx)
{
    struct RastPort rp;
    WORD x, y;

    InitRastPort(&rp);
    rp.BitMap = bm;
    
    for (y = 0; y < HEIGHT; y++)
    {
    	for (x = 0; x < WIDTH; x++)
    	{
    		WORD tile;
    		
    		if (x == 0 || x == (WIDTH - 1) || y == 0 || y == (HEIGHT - 1))
    		{
    			tile = TID_WALL;
    		}
    		else
    		{	
    			tile = TID_FLOOR;
    		}	
			drawTile(gfx, tile, &rp, x << 4, y << 4);
		}	    		
	}
}

int main(int argc, char **argv)
{
    if (openLibs())
    {
        static struct screenData sd;
        static struct boardWindowData bwd; /* Custom window */

        sd.wd = &bwd.wd;

        if (setup("Data/Graphics.iff", &sd, IDCMP_RAWKEY|IDCMP_GADGETDOWN|IDCMP_MOUSEMOVE|IDCMP_MOUSEBUTTONS))
        {
            WoRD *board;

            /* Allocate space for board */
            if (board = allocBoard())
            {   
                if (prepGUI(&sd, &bwd, board))
                {
                    eventLoop(&sd, &bwd.wd);
                    cleanGUI(&bwd);
                }
                FreeVec(board);
            }
            cleanup(&sd);
        }
        closeLibs();
    }
    else
        printf("Couldn't open required libraries V39!\n");
    return(RETURN_OK);
}
