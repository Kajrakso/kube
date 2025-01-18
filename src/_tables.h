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

#endif /* __TABLES_H_ */