#ifndef _MOVES_H_
#define _MOVES_H_

#include "cube.h"
#include <stdio.h>

typedef enum {
  U, D,
  L, R,
  F, B,
} facemove;

extern uint16_t corner_trans[NMOVES][NCORNERCUBIES];
extern uint16_t edge_trans[NMOVES][NEDGECUBIES];

void initialize_move_tables();
void gen_move_tables();
bool save_move_tables(const char *filename);
bool load_move_tables(const char *filename);

void make_move(cube_t* cube, int move);


#endif /* _MOVES_H_ */