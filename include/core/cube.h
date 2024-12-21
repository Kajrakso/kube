#ifndef _CUBE_H_
#define _CUBE_H_

#include <stdint.h>

#define NEDGES 12 /* number of edge cubies */
#define NCORNERS 8 /* number of corner cubies */

/* A cube is represented by an array of 12 edge cubies and an array of 8 corner cubies.
  Each cubie is represented by a 16-bit integer and encodes both the permutation and orientation of the cubie. */
typedef struct cube_t {
  uint16_t edges[NEDGES];
  uint16_t corners[NCORNERS];
} cube_t;

/* Create a new cube with all cubies in their solved state. */
cube_t cube_create_new_cube();

#endif /* _CUBE_H_ */