#ifndef __TABLES_H_
#define __TABLES_H_

#include "_cube.h"
#include "utils.h"
#include "../include/core/move.h"
#include "../include/core/tables.h"
#include <stdio.h>
#include <stdbool.h>


/* moves */
extern uint16_t move_table_corner_transformation[NMOVES][NCORNERCUBIES];
extern uint16_t move_table_edge_transformation[NMOVES][NEDGECUBIES];

/* symmetries */
extern uint16_t sym_table_corner_transformation[NSYMS][NCORNERCUBIES];
extern uint16_t sym_table_edge_transformation[NSYMS][NEDGECUBIES];

/* Applies the symmetry sym to the cube.
This function should probably not be used.
Use `cube_operation_sym_conjugate` for conjugation
by a symmetry.*/
void cube_apply_symm(cube_t* cube, int sym);

/* ----------------------------- implementation ------------------------ */

/* -------- moves ------------ */

const enum facemove;
static const int edge_perm_cycles[NFACES][4];
static const int edge_orient_change[NAXES][NFACES];
static const int corner_perm_cycles[NFACES][4];
static const int corner_orient_change[NAXES][NFACES][4];

static void initialize_move_tables();
static void gen_move_tables();

/* -------- sym ------------ */

// todo: rethink how this should be done. works for now.

static int which_edge_at_pos(int pos, cube_t* cube);
static int which_corner_at_pos(int pos, cube_t* cube);
static void do_y_rot(cube_t* cube);
static void do_z_rot(cube_t* cube);
static void do_inversion(cube_t* cube);

static void initialize_sym_tables();
static void gen_sym_tables();

#endif /* __TABLES_H_ */