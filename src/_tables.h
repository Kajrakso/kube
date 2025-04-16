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

void initialize_move_tables();
void gen_move_tables();
void initialize_sym_tables();
void gen_sym_tables();

//! NOTE: These are for testing!
// void gen_ptable_H();
// void gen_ptable_K();
void gen_ptable_L();

extern int ece_mtable[495][NMOVES];
extern int coud_mtable[2187][NMOVES];
extern int eofb_mtable[2048][NMOVES];

void gen_ece_mtable();
void gen_eofb_mtable();
void gen_coud_mtable();

#endif /* __TABLES_H_ */
