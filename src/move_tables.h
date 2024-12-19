#ifndef _MOVE_TABLES_H_
#define _MOVE_TABLES_H_

#include "cube.h"
#include <stdio.h>

extern uint16_t corner_trans[NMOVES][NCORNERCUBIES];
extern uint16_t edge_trans[NMOVES][NEDGECUBIES];

void initialize_move_tables();
void gen_move_tables();
bool save_move_tables(const char *filename);
bool load_move_tables(const char *filename);

#endif /* _MOVE_TABLES_H_ */