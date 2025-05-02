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

/* a move mask prevents us from checking unnecessary move sequences.
"R R" is the same as "R2". For two subsequent moves on the same axes
we only do one: "R L" and "L R" for instance. */
extern uint32_t move_mask[NMOVES + 1];
extern uint64_t move_table_ccu_index[NCCU][NMOVES];
extern uint64_t move_table_coud_index[NCO][NMOVES];
extern uint64_t move_table_ece_index[NECE][NMOVES];
extern uint64_t move_table_eofb_index[NEO][NMOVES];
extern uint64_t sym_table_e_index[NEO*NECE][NSYMS];

/* todo: need to gen these before using them. */
void gen_move_mask();
void gen_move_table_ccu_index();
void gen_move_table_coud_index();
void gen_move_table_ece_index();
void gen_move_table_eofb_index();
void gen_sym_table_e_index();

void movemask_remove_move(int move);

/* runs through the ptable and counts the number of cosets for each p-value. */
void check_Hdat();

#endif /* __TABLES_H_ */
