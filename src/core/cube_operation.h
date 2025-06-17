#ifndef CUBE_OPERATION_H
#define CUBE_OPERATION_H

#include "cube.h"

/* Check if two cubes are equal. */
bool cube_operation_is_equal(cube_t cube1, cube_t cube2);

/* This function returns the composition of cube2 and cube1 in the Rubik's cube
group. That is, first apply cube1, then cube2. `cube_move_apply_move` should be
used when applying moves to the cube.*/
cube_t cube_operation_compose(cube_t cube1, cube_t cube2);

/* This function returns the inverse of the cube. That is, the cube C^(-1) that
 * satisfies C^(-1)*C = I. */
cube_t cube_operation_inverse(cube_t cube);

/* Conjugate by the cube symmetry S. That is, for the cube `C`, return `S^-1 * C
 * * S`. */
cube_t cube_operation_sym_conjugate(cube_t cube, int symmetry);

#endif /* CUBE_OPERATION_H */
