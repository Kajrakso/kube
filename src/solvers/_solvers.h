#ifndef __SOLVERS_H_
#define __SOLVERS_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/solvers/solvers.h"
#include "../_tables.h"
#include "../core/_cube.h"
#include "../core/_cube_state.h"
#include "_index.h"

void solve_cube(cube_t cube, int max_depth, uint8_t* ptable, int max_num_sols,
                int* solutions);

#endif /* __SOLVERS_H_ */
