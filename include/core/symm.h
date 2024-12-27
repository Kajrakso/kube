#ifndef _SYMM_H_
#define _SYMM_H_

#include "cube.h"

/* Multiply by a symmetry element of Oh on the left. */
void cube_symm_Oh_apply_symmetry(cube_t* cube_t, int symm);

#endif /* _SYMM_H_ */