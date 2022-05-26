
/*
** GameX engine
** Src > Edit
*/

#ifndef EDIT_H
#define EDIT_H

#include "Windows.h"

#define WIDTH 20
#define HEIGHT 16

/* Editor tiles */
enum Tiles
{
    TID_BACK,
    TID_WALL,
    TID_FLOOR,    
    TID_KEY_BOX,
    TID_KEY_STONE,
    TID_SWITCH_BOX,
    TID_SWITCH_STONE,
    TID_SAND_BOX,
    TID_MUD,
    TID_WATER_BOX,
    TID_FIRE,
    TID_HERO,
    TID_BOMB_BOX,
    TID_SKULL,
    TID_CHERRY
};

enum ID
{
    GID_SELECT
};

/* Tile selection */
struct selectData
{
    struct gadgetData gd;
    WORD width, height; /* In tiles */
    WORD selected, prevSelect; /* Selected tile */
    struct windowData *wd;
    struct BitMap *tileGfx; /* Tile graphics */
};

/* Board editing */
struct editData
{
    struct gadgetData gd;
    struct selectData *sd;
    BOOL paint;
    WORD cursX, cursY;
    struct windowData *wd;
    WORD *board; /* Array of tiles */
    struct BitMap *tileGfx;
}

BOOL initSelect(struct selectData *sd, struct gadgetData *prev, WORD left, WORD top, WORD width, WORD height, WORD selected, struct windowData *wd, struct BitMap *gfx);

BOOL initEdit(struct editData *ed, struct gadgetData *prev, WORD left, WORD top, struct windowData *wd, struct BitMap *gfx, WORD *board);

WORD *allocBoard();

#endif /* EDIT_H */
