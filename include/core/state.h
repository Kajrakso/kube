#ifndef _STATE_H_
#define _STATE_H_

#include "cube.h"

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

#endif /* _STATE_H_ */