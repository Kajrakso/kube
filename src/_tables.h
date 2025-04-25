#ifndef __TABLES_H_
#define __TABLES_H_

#include "_cube.h"
#include "_move.h"
#include "_sym.h"
#include "_index.h"
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

//! 19.04.2025: These are for testing!

void ptable_set_val(uint64_t i, uint8_t p, uint8_t* ptable);
uint8_t ptable_read_val(uint64_t i, uint8_t* ptable);

extern uint64_t e_stable[NEO*NECE][NSYMS];
extern uint64_t ece_mtable[NECE][NMOVES];
extern uint64_t eofb_mtable[NEO][NMOVES];
extern uint64_t coud_mtable[NCO][NMOVES];
extern uint64_t ccu_mtable[NCCU][NMOVES];

void gen_ece_mtable();
void gen_eofb_mtable();
void gen_coud_mtable();
void gen_ccu_mtable();
void gen_e_stable();

#endif /* __TABLES_H_ */
