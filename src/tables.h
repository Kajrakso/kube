#ifndef TABLES_H
#define TABLES_H

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include "core/cube.h"
#include "core/move.h"
#include "core/sym.h"
#include "utils.h"
#include "env.h"

#define FACTORIAL8 40320

/* type used to store information about each
 * type of pruning table. */
typedef struct ptable_info_t {
    char name[FILENAME_MAX];
    unsigned long long ptable_size; /* in bytes */
    unsigned long long number_of_elements; /* can be larger than ptable_size */
    char filename[FILENAME_MAX];
    uint64_t (*cube_to_index_func) (cube_t* cube, axes ax);
    void (*gen_ptable_func)(void);
    uint8_t (*read_value_ptable_func)(uint64_t i, uint8_t* ptable);
    void (*set_value_ptable_func)(uint64_t i, uint8_t p, uint8_t* ptable);
    bool ptable_is_loaded;
    void* ptable;
    /* moveset? */
    /* prune function? */
} ptable_data_t;

/* moves */
extern uint16_t move_table_corner_transformation[NMOVES][NCORNERCUBIES];
extern uint16_t move_table_edge_transformation[NMOVES][NEDGECUBIES];

/* symmetries */
extern uint16_t sym_table_corner_transformation[NSYMS][NCORNERCUBIES];
extern uint16_t sym_table_edge_transformation[NSYMS][NEDGECUBIES];

void initialize_move_tables();
void gen_move_tables();
void initialize_sym_tables();
void gen_sym_tables();

//! 19.04.2025: These are for testing!

struct c_index_cclass_sym {
  uint64_t cclass_i;
  uint64_t cclass;
  int sym;
};

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

bool save_table_to_file(const char* path, void* table, const size_t table_size);

/* Get the tables. Returns NULL if not loaded. */
void* get_sym_table_e_index();
void* get_dr_subsets();

/* Initializes the move and symmetry tables.
You have to call this function before applying moves to the cube.*/
void cube_tables_generate();

/* Call before using ptables and sym_table_e_index*/
int cube_tables_load();

/* Call after use of ptables and sym_table_e_index */
void cube_tables_free();


void free_ptable(ptable_data_t* ptable_data);

// todo: WIP
void gen_ptable_opt1();
void gen_ptable_DR();

/* runs through the ptable and counts the number of cosets for each p-value. */
void analyze_ptable(ptable_data_t ptable_data);

int cube_tables_load_ptable(ptable_data_t* ptable_data);

int cube_tables_load_sym_table_e_index();
int cube_tables_load_dr_subsets();

bool parse_cp_to_dr_subset_file_and_save_dr_subset_table(char* filename);


/* set and read value from ptable.
 * these are needed since we store 2 values per byte. */
static inline void ptable_set_val_2_values_per_byte(uint64_t i, uint8_t p, uint8_t* ptable) {
  if (i % 2 == 0) {
    ptable[i >> 1] = (ptable[i >> 1] & 0xF0) | (p & 0x0F);
  } else {
    ptable[i >> 1] = (ptable[i >> 1] & 0x0F) | ((p & 0x0F) << 4);
  }
}

static inline uint8_t ptable_read_val_2_values_per_byte(uint64_t i, uint8_t* ptable) {
  uint8_t val = ptable[i >> 1];
  return (val >> ((i & 1) * 4)) & 0x0F;
}

#endif /* TABLES_H */
