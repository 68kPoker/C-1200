
/*
** GameX engine
** Src > Edit
*/

#ifndef EDIT_H
#define EDIT_H

#include "Windows.h"

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

BOOL initSelect(struct selectData *sd, struct gadgetData *prev, WORD left, WORD top, WORD width, WORD height, WORD selected, struct windowData *wd, struct BitMap *gfx);

#endif /* EDIT_H */
