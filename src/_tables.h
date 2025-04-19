#ifndef __TABLES_H_
#define __TABLES_H_

#include "_cube.h"
#include "_move.h"
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


void ptable_set_val(uint32_t i, uint8_t p, uint8_t* ptable);
uint8_t ptable_read_val(uint32_t i, uint8_t* ptable);

void gen_ptable_L();

extern uint32_t ece_mtable[NECE][NMOVES];
extern uint32_t coud_mtable[NCO][NMOVES];
extern uint32_t eofb_mtable[NEO][NMOVES];

void gen_ece_mtable();
void gen_eofb_mtable();
void gen_coud_mtable();

void
DLS_L(
  uint32_t ece,
  uint32_t eofb,
  uint32_t coud,
  unsigned int p,
  int depth,
  int prev_move,
  int num_moves_done,
  uint8_t* ptable
);


void
gen_ptable_L();

#endif /* __TABLES_H_ */
