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

/* pruning */
extern uint16_t prune_table_cp[FACT8];

/* Applies the symmetry sym to the cube.
This function should probably not be used.
Use `cube_operation_sym_conjugate` for conjugation
by a symmetry.*/
void cube_apply_symm(cube_t* cube, int sym);

void initialize_move_tables();
void gen_move_tables();
void initialize_sym_tables();
void gen_sym_tables();

#endif /* __TABLES_H_ */