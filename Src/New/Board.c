
#include <intuition/screens.h>

#include <exec/types.h>

#include <stdio.h>
#include "debug.h"

#include "Board.h"
#include "Blit.h"

/* Scan objects and draw board */

void scanObjects(struct board *b)
{
    WORD i;
    
    for (i = 0; i < b->objects; i++)
    {
        struct object *o = &b->obj[i];
        
        if (o->shift > 0)
        {
            struct tile *t = &b->tiles[o->y][o->x], *s = &b->tiles[o->y - o->dy][o->x - o->dx];
            
            if (o->kind == O_HERO)
            {   
                o->type = 30;
                
                if (o->dx > 0)
                {
                    o->type += 2;
                }    
                else if (o->dy > 0)
                {
                    o->type += 4;
                }
                else if (o->dy < 0)
                {    
                    o->type += 6;
                }    
                o->type += (o->shift >> 3) & 1;
            }
            
            if ((o->shift -= SPEED) == 0)
            {
                s->out = 0;
                
                if (t->kind == T_CRACKED && t->type == 8)
                {
                    t->in = 0;
                    t->type++;
                    t->kind = T_WALL;
                }    
            }
            t->update[0] = t->update[1] = TRUE; /* Redraw */
            s->update[0] = s->update[1] = TRUE; /* Redraw */
        }        
        else if (o->shift == 0)
        {        
            /* Check if triggered to move */
            if ((o->tx != 0) ^ (o->ty != 0))
            {   
                struct tile *t = &b->tiles[o->y][o->x], *s = &b->tiles[o->y + o->ty][o->x + o->tx];
                
                /* Check if movement possible */
                
                if (s->kind == T_WALL)
                {
                    continue;
                }    
                
                if (s->in)
                {
                    /* Object, push */
                    struct object *pushed = b->obj + s->in - 1;
                    struct tile *v = &b->tiles[o->y + o->ty + o->ty][o->x + o->tx + o->tx];
                    
                    if (v->kind == T_WALL || v->in != 0)
                    {
                        continue;
                    }    
                    
                    if (v->kind == T_CRACKED)
                    {
                        v->type++;
                    }
                    
                    pushed->shift = 16;
                    
                    if (s->in < i + 1)
                    {
                        pushed->shift -= SPEED;
                    }    
                    
                    pushed->x += pushed->dx = o->tx;
                    pushed->y += pushed->dy = o->ty;
                    
                    s->out = s->in;
                    v->in = s->in;
                }    
                
                o->x += o->dx = o->tx;
                o->y += o->dy = o->ty;
                
                o->shift = 16 - SPEED;
                
                t->out = s->in = i + 1;
                t->in = 0;
                
                t->update[0] = t->update[1] = TRUE; /* Redraw */
                s->update[0] = s->update[1] = TRUE; /* Redraw */
            }
        }
    }
}    

void drawBoard(struct board *b)
{
    WORD x, y;
    
    for (y = 0; y < HEIGHT; y++)
    {
        for (x = 0; x < WIDTH; x++)
        {
            struct tile *t = &b->tiles[y][x];
            
            if (t->type >= 9 && t->type < 11)
            {
                if (++t->anim == 4)
                {
                    t->type++;
                    t->anim = 0;
                    t->update[0] = t->update[1] = TRUE;
                }    
            }    
            
            if (t->update[b->frame])
            {
                TILE f;
                    
                f.gfx = b->s->gfx;
                f.gfxX = t->type % 20;
                f.gfxY = t->type / 20;
            
                t->update[b->frame] = FALSE;
                if (!t->in && !t->out)
                {
                    /* Only floor, use fast blit */
                    bltTile(&f, &b->s->s->RastPort, x, y);
                }
                else 
                {
#ifndef SKIP                
                    bltTile(&f, &b->s->s->RastPort, x, y);
#endif                

                    f.gfx = b->s->s->RastPort.BitMap;
                    f.gfxX = x;
                    f.gfxY = y;
            
                    BOOL drawn = FALSE;
                    if (t->in)
                    {
                        struct object *in = &b->obj[t->in - 1];
                        TILE o;
                        o.gfx = b->s->gfx;
                        o.gfxX = in->type % 20;
                        o.gfxY = in->type / 20;
                        
                        /* Draw entering object */
                        
                        if (in->dx == 1)
                        {
                            bltAddTile(&f, &o, -in->shift, HORIZ, b->s->mask, &b->s->s->RastPort, x, y);
                        }
                        else if (in->dx == -1)
                        {       
                            bltAddTile(&f, &o, in->shift, HORIZ, b->s->mask, &b->s->s->RastPort, x, y);
                        }
                        else if (in->dy == 1)
                        {
                            bltAddTile(&f, &o, -in->shift, VERT, b->s->mask, &b->s->s->RastPort, x, y);
                        }
                        else
                        {
                            bltAddTile(&f, &o, in->shift, VERT, b->s->mask, &b->s->s->RastPort, x, y);
                        }
                        drawn = TRUE;
                    }
                    if (t->out)
                    {    
                        struct object *out = &b->obj[t->out - 1];
                        TILE o;
                        o.gfx = b->s->gfx;
                        o.gfxX = out->type % 20;
                        o.gfxY = out->type / 20;
                                                
                        /* Draw leaving object */
                        if (out->dx == 1)
                        {
                            bltAddTile(&f, &o, 16 - out->shift, HORIZ, b->s->mask, &b->s->s->RastPort, x, y);
                        }
                        else if (out->dx == -1)
                        {       
                            bltAddTile(&f, &o, -(16 - out->shift), HORIZ, b->s->mask, &b->s->s->RastPort, x, y);
                        }
                        else if (out->dy == 1)
                        {
                            bltAddTile(&f, &o, 16 - out->shift, VERT, b->s->mask, &b->s->s->RastPort, x, y);
                        }
                        else
                        {
                            bltAddTile(&f, &o, -(16 - out->shift), VERT, b->s->mask, &b->s->s->RastPort, x, y);
                        }                        
                        
                        drawn = TRUE;
                    }
                }
            }    
        }
    }    
}

void setObject(BOARD *b, WORD i, WORD kind, WORD type, WORD x, WORD y)
{
    struct object *o = b->obj + i;
    
    o->kind = kind;    
    o->type = type;
    o->x = x;
    o->y = y;
    b->objects++;
    
    b->tiles[y][x].in = i + 1;
}
