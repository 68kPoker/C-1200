
/*
** GameC engine
** Blitter
**
** Blitter low-level drawing functions
*/

#ifndef BLITTER_H
#define BLITTER_H

#include <exec/types.h>
#include <graphics/gfx.h>

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

/* The following functions use fast aligned blitting if possible */

/* clipBlit: 
 *
 * Copy from source RastPort to destination RastPort.
 * 
 * srp:     Source RastPort
 * sx:      Source X pixel position
 * sy:      Source Y pixel position
 * drp:     Destination RastPort
 * dx:      Destination X pixel position
 * dy:      Destination Y pixel position
 * width:   Width in pixels
 * height:  Height in pixels
 * minterm: Logic operation on bitplanes
 * 
 * Result:  none
 */

VOID clipBlit(struct RastPort *srp, WORD sx, WORD sy, struct RastPort *drp, WORD dx, WORD dy, WORD width, WORD height, UBYTE minterm);

/* bltBitMapRastPort: 
 *
 * Copy from source BitMap to destination RastPort.
 * 
 * sbm:     Source BitMap
 * sx:      Source X pixel position
 * sy:      Source Y pixel position
 * drp:     Destination RastPort
 * dx:      Destination X pixel position
 * dy:      Destination Y pixel position
 * width:   Width in pixels
 * height:  Height in pixels
 * minterm: Logic operation on bitplanes
 * 
 * Result:  none
 */

VOID bltBitMapRastPort(struct BitMap *sbm, WORD sx, WORD sy, struct RastPort *drp, WORD dx, WORD dy, WORD width, WORD height, UBYTE minterm);

/* bltBitMap
 *
 * Copy from source BitMap to destination BitMap.
 * 
 * sbm:     Source BitMap
 * sx:      Source X pixel position
 * sy:      Source Y pixel position
 * dbm:     Destination BitMap
 * dx:      Destination X pixel position
 * dy:      Destination Y pixel position
 * width:   Width in pixels
 * height:  Height in pixels
 * minterm: Logic operation on bitplanes
 * 
 * Result:  none
 */

VOID bltBitMap(struct BitMap *sbm, WORD sx, WORD sy, struct BitMap *dbm, WORD dx, WORD dy, WORD width, WORD height, UBYTE minterm);

/* bltMaskBitMapRastPort: 
 *
 * Copy from source BitMap to destination RastPort through mask.
 * 
 * sbm:     Source BitMap
 * sx:      Source X pixel position
 * sy:      Source Y pixel position
 * drp:     Destination RastPort
 * dx:      Destination X pixel position
 * dy:      Destination Y pixel position
 * width:   Width in pixels
 * height:  Height in pixels
 * minterm: Logic operation
 * mask:    Mask bitplane (must be size of source bitmap)
 * 
 * Result:  none
 */

VOID bltMaskBitMapRastPort(struct BitMap *src, WORD sx, WORD sy, struct RastPort *rp, WORD dx, WORD dy, WORD width, WORD height, UBYTE minterm, PLANEPTR mask);

#endif /* BLITTER_H */
