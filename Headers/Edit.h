
/*
** GameX engine
** Src > Edit
*/

#ifndef EDIT_H
#define EDIT_H

#include "Windows.h"
#include "Board.h"

#include <exec/types.h>

#define WIDTH 20
#define HEIGHT 16

enum ID
{
    GID_SELECT,
    GID_BOARD,
    GID_MENU
};

enum
{
	GID_LOAD,
	GID_CANCEL,
	GID_SAVE,
	GID_PREV,
	GID_NEXT
};	

enum Actions
{
    AID_NONE,
    AID_LOAD,
    AID_CANCEL,
    AID_SAVE,
    AID_PREV,
    AID_NEXT
};

/* Tile selection */
struct selectData
{
    struct gadgetData gd;
    WORD width, height; /* In tiles */
    WORD selected, prevSelected; /* Selected tile */
    struct windowData *wd;
    struct screenData *screen; /* Tile graphics */
};

/* Board editing */
struct editData
{
    struct gadgetData gd;
    struct selectData *sd;
    BOOL paint;
    WORD cursX, cursY;
    struct windowData *wd;
    board *board; /* Array of tiles */
    struct screenData *screen;
};

struct menuGadgetData
{
	struct gadgetData gd;
	struct editData *ed;
};

struct menuData
{
    struct windowData wd;
    WORD action;
    WORD level;
};

BOOL initSelect(struct selectData *sd, struct gadgetData *prev, WORD left, WORD top, WORD width, WORD height, WORD selected, struct windowData *wd, struct screenData *sd);

BOOL initEdit(struct editData *ed, struct gadgetData *prev, WORD left, WORD top, struct windowData *wd, struct screenData *screen, board *board);

WORD *allocBoard();

VOID hitMenu(struct menuGadgetData *mgd, struct IntuiMessage *msg);

#endif /* EDIT_H */
