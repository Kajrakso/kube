#ifndef MOVE_H
#define MOVE_H

#include <stddef.h>

#include "cube.h"

/* These moves correspond to standard Singmaster notation as follows:
    U1 - U, U2 - U2, U3 - U', etc.*/
enum move {
    U1,
    U2,
    U3,
    D1,
    D2,
    D3,
    L1,
    L2,
    L3,
    R1,
    R2,
    R3,
    F1,
    F2,
    F3,
    B1,
    B2,
    B3,
    NULLMOVE
};

/* Applies a move to a cube.
See the enum move for the possible moves. */
void cube_move_apply_move(cube_t* cube, int move);

/* Applies multiple moves to a cube.
See the enum move for the possible moves. */
void cube_move_apply_multiple_moves(cube_t* cube, int* moves_arr, size_t number_of_moves);

/* Use `cube_move_apply_move` or `cube_move_apply_multiple_moves` directly if
performance is critical.

Example usage:

```C
const char* moves = "R U R' U' R' F R2 U' R' U' R U R' F'";
cube_move_apply_move_string(&cube, moves);
```*/
bool cube_move_apply_move_string(cube_t* cube, const char* moves);

int get_inv_move(int m);

#endif /* MOVE_H */
