
#include <exec/types.h>

#include "Screen.h"

#define MAX_OBJECTS 20

#define SPEED 2

#define WIDTH  20
#define HEIGHT 16

enum
{
    T_BACK,
    T_WALL,
    T_FLOOR,
    T_CRACKED
};

enum
{
    O_HERO,
    O_BOX
};    

struct object
{
    WORD type;
    WORD kind;
    WORD x, y; /* Object position */
    BYTE shift; /* 16-0 value */
    BYTE dx, dy; /* Object dir */
    BYTE tx, ty;
    BOOL active;
};

struct tile
{
    WORD type; /* Floor type */
    WORD kind;
    WORD in, out; /* Optional entering and leaving object(s) */
    BOOL update[2]; /* Update in both buffers */
    WORD anim;
};    

typedef struct board
{
    struct object obj[MAX_OBJECTS];
    struct tile tiles[HEIGHT][WIDTH];
    WORD objects;
    WORD frame;
    SCREEN *s;
} BOARD;

void scanObjects(struct board *b);
void drawBoard(struct board *b);
void setObject(BOARD *b, WORD i, WORD kind, WORD type, WORD x, WORD y);
