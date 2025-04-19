#ifndef __INDEX_H_
#define __INDEX_H_

#include "utils.h"
#include "_cube.h"

// testing
/* TODO: reconsider the way to calculate these indeces. It need to be fast. */
extern int combinatorials_lookup[20736];

uint32_t
cube_to_ece_index(cube_t* cube);

uint32_t
cube_to_coud_index(cube_t* cube);

uint32_t
cube_to_eofb_index(cube_t* cube);

uint32_t
cube_to_L_index(cube_t* cube);

uint32_t
ece_eofb_coud_to_L_index(uint32_t, uint32_t, uint32_t);

// helper functions

int which_corner_at_pos(int pos, cube_t* cube);
int which_edge_at_pos(int pos, cube_t* cube);




/*

nxopt:

factor of the index     // subgroup
9930                    // permuting U and D layer corners with fixed orientation
* (1, 24, 70, 1680)     // ep: () <=> (e-edges/UD-edges, UD-edges, U-edges/D-edges/e-edges, U-edges/D-edges)
* (16, 256, 2048)       // eo: (2**4, 2**8, 2**11) <=> (e-edges, UD-edges, all edges)
* 495                   // e-slice: keep the e-slice edges in the e-slice

My L group:
solved CO
arbitrary CP
solved EO
arbitrary EP, except: keep e-slice in e-slice
*/

#endif /* __INDEX_H_ */
