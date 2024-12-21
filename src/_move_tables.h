#ifndef _MOVE_TABLES_H_
#define _MOVE_TABLES_H_

#include "_cube.h"
#include "../include/core/move.h"
#include <stdio.h>
#include <stdbool.h>

extern uint16_t move_table_corner_transformation[NMOVES][NCORNERCUBIES];
extern uint16_t move_table_edge_transformation[NMOVES][NEDGECUBIES];

#endif /* _MOVE_TABLES_H_ */