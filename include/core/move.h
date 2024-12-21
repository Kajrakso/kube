#ifndef _MOVE_H_
#define _MOVE_H_

#include "cube.h"
#include "movetables.h"

#include <stddef.h>

/* These moves correspond to standard Singmaster notation as follows:
    
    U1 - U
    U2 - U2
    U3 - U'

etc.*/
enum move {
    U1, U2, U3,
    D1, D2, D3,
    L1, L2, L3,
    R1, R2, R3,
    F1, F2, F3,
    B1, B2, B3,
};

/* Applies a move to a cube.
See the enum move for the possible moves.

Example usage:

```C
cube_move_apply_move(&cube, R1);
```*/
void cube_move_apply_move(cube_t* cube, int move);

/* Applies multiple moves to a cube.
See the enum move for the possible moves. */
void cube_move_apply_multiple_moves(cube_t* cube, int* moves_arr, size_t number_of_moves);

/* Use `cube_move_apply_move` or `cube_move_apply_multiple_moves` directly if performance is critical.

Example usage: 

```C
const char* moves = "R U R' U' R' F R2 U' R' U' R U R' F'";
cube_move_apply_move_string(&cube, moves);
```*/
void cube_move_apply_move_string(cube_t* cube, const char* moves);

#endif /* _MOVE_H_ */