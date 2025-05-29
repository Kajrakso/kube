#ifndef __TABLES_H_
#define __TABLES_H_

#include <stdbool.h>
#include <stdio.h>

#include "../include/core/move.h"
#include "../include/core/tables.h"
#include "_sym.h"
#include "core/_cube.h"
#include "core/_move.h"
#include "utils.h"

#define SIZEH 10066636800ULL
#define SIZE_PTABLE_H (5033318400ULL * (sizeof(uint8_t)))

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

bool save_table_to_file(const char* path, void* table, const size_t table_size);

int init_ptable_H(const char* path);
void* get_ptable_H();
void free_ptable_H();

int init_sym_table_e_index(const char* path);
void* get_sym_table_e_index();
void free_sym_table_e_index();

//! 19.04.2025: These are for testing!

struct c_index_cclass_sym {
  uint64_t cclass_i;
  uint64_t cclass;
  int sym;
};

static inline void ptable_set_val(uint64_t i, uint8_t p, uint8_t* ptable) {
  if (i % 2 == 0) {
    ptable[i >> 1] = (ptable[i >> 1] & 0xF0) | (p & 0x0F);
  } else {
    ptable[i >> 1] = (ptable[i >> 1] & 0x0F) | ((p & 0x0F) << 4);
  }
}

static inline uint8_t ptable_read_val(uint64_t i, uint8_t* ptable) {
  uint8_t val = ptable[i >> 1];
  return (val >> ((i & 1) * 4)) & 0x0F;
}

/* a move mask prevents us from checking unnecessary move sequences.
"R R" is the same as "R2". For two subsequent moves on the same axes
we only do one: "R L" and "L R" for instance. */
extern uint32_t move_mask[NMOVES + 1];
extern uint64_t move_table_ccu_index[NCCU][NMOVES];
extern uint64_t move_table_coud_index[NCO][NMOVES];
extern uint64_t move_table_ece_index[NECE][NMOVES];
extern uint64_t move_table_eofb_index[NEO][NMOVES];
extern struct c_index_cclass_sym cclass_table[NCCU * NCO];

/* todo: need to gen these before using them. */
void gen_move_mask();
void gen_move_table_ccu_index();
void gen_move_table_coud_index();
void gen_move_table_ece_index();
void gen_move_table_eofb_index();
void gen_sym_table_e_index();
void gen_c_sym_index_tables();

void movemask_remove_move(int move);

/* runs through the ptable and counts the number of cosets for each p-value. */
void check_Hdat();

#endif /* __TABLES_H_ */
