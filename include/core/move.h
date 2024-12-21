#ifndef _MOVE_H_
#define _MOVE_H_

#include "cube.h"
#include <stdbool.h>

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

/* Initializes the move tables.
You have to call this function before applying moves to the cube.*/
void generate_move_tables();

/* Saves the move tables to disk. */
bool save_move_tables(const char *filename);

/* Loads the move tables from a file. */
bool load_move_tables(const char *filename);


/* Applies a move to a cube.
See the enum move for the possible moves. */
void cube_apply_move(cube_t* cube, int move);


/* Applies multiple moves to a cube.
See the enum move for the possible moves. */
void cube_apply_multiple_moves(cube_t* cube, int* moves_arr, int number_of_moves);

#endif /* _MOVE_H_ */