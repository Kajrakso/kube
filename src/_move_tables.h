#ifndef __MOVETABLES_H_
#define __MOVETABLES_H_

#include "_cube.h"
#include "../include/core/move.h"
#include "../include/core/movetables.h"
#include <stdio.h>
#include <stdbool.h>

extern uint16_t move_table_corner_transformation[NMOVES][NCORNERCUBIES];
extern uint16_t move_table_edge_transformation[NMOVES][NEDGECUBIES];

#endif /* __MOVETABLES_H_ */