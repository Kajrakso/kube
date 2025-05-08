#ifndef __SOLVERS_H_
#define __SOLVERS_H_

#include "_cube_state.h"
#include "_index.h"
#include "_cube.h"
#include "_tables.h"
#include "../include/solvers/solvers.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "_print_cube.h"

void
solve_cube(
    cube_t cube,
    int max_depth,
    uint8_t* ptable,
    int max_num_sols,
    int* solutions
);

#endif /* __SOLVERS_H_ */
