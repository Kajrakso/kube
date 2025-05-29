#ifndef __SYM_H_
#define __SYM_H_

#include "core/_cube.h"
#include "../include/core/move.h"

void do_y_rot(cube_t* cube);
void do_z_rot(cube_t* cube);
void do_inversion(cube_t* cube);

int
get_inv_sym(int s);

void
cube_apply_symm(cube_t* cube, int sym);

#endif /* __SYM_H_ */ 
