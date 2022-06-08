
/*
** Silnik C-1200
*/

/* Pole na planszy gry Magazyn */

/* Udost�pnione elementy */

#include <exec/types.h>

/* Domy�lny rozmiar */

#define COLS 20
#define ROWS 16

enum /* Podstawowe kafle */
{
    BACK,
    WALL,
    FLOOR,
    FLAGSTONE
    BASIC_TILES
};

enum /* Podstawowe obiekty */
{
    NONE,
    HERO,
    BOX,
    BASIC_OBJECTS
};

struct TileLayer
{
    UBYTE type, id;
};

struct Tile
{
    struct TileLayer floor, object;
};

extern struct TileAPI
{
    void (*init)(); /* Przygotuj API do pracy */
    void (*clear)(struct Tile *t); /* Wyczy�� tablic� kafli */
    struct Tile *(*alloc)(); /* Zaalokuj plansz� z�o�on� z kafli */
    



} tile;
