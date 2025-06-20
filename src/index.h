#ifndef INDEX_H
#define INDEX_H

#include <stdio.h>

#include "core/cube.h"
#include "tables.h"
#include "utils.h"

// testing
/* TODO: reconsider the way to calculate these indeces. It need to be fast. */

extern int ece_combinatorials_lookup[20736];
extern int ccu_combinatorials_lookup[4096];

uint64_t cube_to_ccu_index(cube_t* cube);

uint64_t cube_to_ece_index(cube_t* cube);

uint64_t cube_to_coud_index(cube_t* cube);

uint64_t cube_to_eofb_index(cube_t* cube);

uint64_t cclass_i_e_to_H_index(uint64_t cclass_i, uint64_t e_i);

uint64_t ccu_coud_to_c_index(uint64_t ccu, uint64_t coud);

uint64_t ece_eofb_to_e_index(uint64_t ece, uint64_t eofb);

uint64_t cube_to_c_index(cube_t* cube);

uint64_t cube_to_e_index(cube_t* cube);

uint64_t cube_to_H_index(cube_t* cube, axes ax);

/* NOTE: the cube returned is not necessarily a valid cube.
 * Also, these function is not optimized. */
cube_t coud_index_to_cube(uint64_t coud_i);

cube_t ccu_index_to_cube(uint64_t ccu_i);

cube_t c_index_to_cube(uint64_t c_i);

/* TEMP: 2025-06-16 testing */

uint64_t cube_to_cofb_index(cube_t* cube);
uint64_t cube_to_colr_index(cube_t* cube);
uint64_t cube_to_eoud_index(cube_t* cube);
uint64_t cube_to_eolr_index(cube_t* cube);
uint64_t cube_to_ccl_index(cube_t* cube);
uint64_t cube_to_ccb_index(cube_t* cube);
uint64_t cube_to_ecs_index(cube_t* cube);
uint64_t cube_to_ecm_index(cube_t* cube);

// uint64_t cube_to_H_lr_index(cube_t* cube);
// uint64_t cube_to_H_fb_index(cube_t* cube);
#endif /* INDEX_H */
