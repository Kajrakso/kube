#ifndef CUBE_STATE_H
#define CUBE_STATE_H

#include "cube.h"

bool cube_valid_eo(cube_t* cube);

bool cube_valid_co(cube_t* cube);

// todo: implement this.
bool cube_valid_permutation(cube_t* cube);

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

/* Check if the cube has corner permutation solved. */
bool cube_state_is_cp(cube_t* cube);

/* Check if the cube is in edge orientation on any exis. */
bool cube_state_is_eo(cube_t* cube);

/* Check if the cube has edge permutation solved. */
bool cube_state_is_ep(cube_t* cube);

/* Check if the cube is in domino reduction on any axis. */
bool cube_state_is_dr(cube_t* cube);

/* Check if the cube is in half turn reduction. */
bool cube_state_is_htr(cube_t* cube);

/* Check if the cube is solved.  */
bool cube_state_is_solved(cube_t* cube);

/* Check if the cube has solved corners. */
bool cube_state_is_corners_solved(cube_t* cube);

/* Check if the cube has solved edges. */
bool cube_state_is_edges_solved(cube_t* cube);

#endif /* CUBE_STATE_H */
