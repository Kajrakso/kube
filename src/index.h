#ifndef INDEX_H
#define INDEX_H

#include <stdio.h>

#include "core/cube.h"
#include "tables.h"
#include "utils.h"

/* TODO: reconsider the way to calculate these indeces. It need to be fast. */

// we need a way to calculate the combinatorials
// for edges and corners. for now we just precompute
// these and look them up.
extern int ece_combinatorials_lookup[20736];
extern int ccu_combinatorials_lookup[4096];

// When rotating the cube to another axis
// the new positions of the pieces relative
// to the new axis is encoded in these tables.
extern int i_transform_axes_c[NAXES][NCORNERS];
extern int i_transform_axes_e[NAXES][NEDGES];

// the four "base" indeces
uint64_t cube_to_cc_index(cube_t* cube, axes ax);

uint64_t cube_to_ec_index(cube_t* cube, axes ax);

uint64_t cube_to_co_index(cube_t* cube, axes ax);

uint64_t cube_to_eo_index(cube_t* cube, axes ax);

// combining two indeces
uint64_t cc_co_to_c_index(uint64_t cc, uint64_t co);

uint64_t ec_eo_to_e_index(uint64_t ec, uint64_t eo);

uint64_t cclass_i_e_to_H_index(uint64_t cclass_i, uint64_t e_i);

// the main index
uint64_t cube_to_H_index(cube_t* cube, axes ax);

uint64_t cube_to_c_index(cube_t* cube, axes ax);

uint64_t cube_to_e_index(cube_t* cube, axes ax);

/* NOTE: the cube returned is not necessarily a valid cube.
 * Also, these function are not optimised. */
cube_t coud_index_to_cube(uint64_t coud_i);

cube_t ccu_index_to_cube(uint64_t ccu_i);

cube_t c_index_to_cube(uint64_t c_i);

#endif /* INDEX_H */
