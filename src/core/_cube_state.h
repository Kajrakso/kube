#ifndef __CUBE_STATE_H
#define __CUBE_STATE_H

#include "../../include/core/state.h"
#include "_cube.h"

bool cube_valid_eo(cube_t* cube);
bool cube_valid_co(cube_t* cube);

// todo: implement this.
bool cube_valid_permutation(cube_t* cube);

#endif /* __CUBE_STATE_H */
