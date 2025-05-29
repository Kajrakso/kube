#ifndef __SOLVERS_H_
#define __SOLVERS_H_

#include "../core/_cube_state.h"
#include "_index.h"
#include "../core/_cube.h"
#include "../_tables.h"
#include "../../include/solvers/solvers.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void
solve_cube(
    cube_t cube,
    int max_depth,
    uint8_t* ptable,
    int max_num_sols,
    int* solutions
);

#endif /* __SOLVERS_H_ */
