#ifndef SYM_H
#define SYM_H

#include "cube.h"
#include "cube_operation.h"

void do_y_rot(cube_t* cube);
void do_z_rot(cube_t* cube);
void do_inversion(cube_t* cube);

int get_inv_sym(int s);

/* Applies the symmetry sym to the cube.
This function should probably not be used.
Use `cube_operation_sym_conjugate` for conjugation
by a symmetry.*/
void cube_apply_symm(cube_t* cube, int sym);

#endif /* SYM_H */
