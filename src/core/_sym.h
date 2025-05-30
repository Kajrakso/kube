#ifndef __SYM_H_
#define __SYM_H_

#include "../../include/core/move.h"
#include "_cube.h"

void do_y_rot(cube_t* cube);
void do_z_rot(cube_t* cube);
void do_inversion(cube_t* cube);

int get_inv_sym(int s);

/* Applies the symmetry sym to the cube.
This function should probably not be used.
Use `cube_operation_sym_conjugate` for conjugation
by a symmetry.*/
void cube_apply_symm(cube_t* cube, int sym);

#endif /* __SYM_H_ */
