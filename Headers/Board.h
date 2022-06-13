
/* Functions that work on BoardData */

#include <stdio.h>

long resetBoard(struct boardData *bd, struct gameData *gd, struct programData *pd);

long replaceHero(unsigned int tileX, unsigned int tileY, struct boardData *bd, struct gameData *gd, struct programData *pd);

long scanBoard(struct boardData *bd, struct gameData *gd, struct programData *pd);

long drawBoard(FILE *f, struct boardData *bd, struct gameData *gd, struct programData *pd);
