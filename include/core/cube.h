#ifndef _CUBE_H_
#define _CUBE_H_

#include <stdint.h>
#include <stdbool.h>

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

/* Check if the cube has edge orientation on FB-axis. */
bool cube_state_is_eo_FB(cube_t* cube);

/* Check if the cube has edge orientation on LR-axis. */
bool cube_state_is_eo_LR(cube_t* cube);

/* Check if the cube has edge orientation on UD-axis. */
bool cube_state_is_eo_UD(cube_t* cube);

/* Check if the cube has corner orientation on FB-axis. */
bool cube_state_is_co_FB(cube_t* cube);

/* Check if the cube has corner orientation on LR-axis. */
bool cube_state_is_co_LR(cube_t* cube);

/* Check if the cube has corner orientation on UD-axis. */
bool cube_state_is_co_UD(cube_t* cube);

/* Check if the cube is in domino reduction on FB-axis. */
bool cube_state_is_dr_FB(cube_t* cube);

/* Check if the cube is in domino reduction on LR-axis. */
bool cube_state_is_dr_LR(cube_t* cube);

/* Check if the cube is in domino reduction on UD-axis. */
bool cube_state_is_dr_UD(cube_t* cube);

/* Check if the cube is in corner orientation on any axis. */
bool cube_state_is_co(cube_t* cube);

/* Check if the cube is in edge orientation on any exis. */
bool cube_state_is_eo(cube_t* cube);

/* Check if the cube is in domino reduction on any axis. */
bool cube_state_is_dr(cube_t* cube);

/* Check if the cube is in half turn reduction. */
bool cube_state_is_htr(cube_t* cube);

/* Check if the cube is solved. We assume that the cube is in a valid state. */
bool cube_state_is_solved(cube_t* cube);

#endif /* _CUBE_H_ */