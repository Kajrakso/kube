#ifndef __INDEX_H_
#define __INDEX_H_

#include "utils.h"
#include "_cube.h"

// testing
extern int combinatorials_lookup[20736];

int
cube_to_ece_index(cube_t* cube);

int
cube_to_coud_index(cube_t* cube);

int
cube_to_eofb_index(cube_t* cube);

unsigned int
cube_to_L_index(cube_t* cube);

#endif /* __INDEX_H_ */
