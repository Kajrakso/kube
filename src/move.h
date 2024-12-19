#ifndef _MOVE_H_
#define _MOVE_H_

#include "cube.h"
#include "move_tables.h"
#include <stdio.h>

typedef enum {
    U1, U2, U3,
    D1, D2, D3,
    L1, L2, L3,
    R1, R2, R3,
    F1, F2, F3,
    B1, B2, B3,
} move;

void make_move(cube_t* cube, move move);
void make_mult_moves(cube_t* cube, move* moves_ptr, int number_of_moves);

#endif /* _MOVE_H_ */