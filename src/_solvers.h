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

bool
solve_cube(
    cube_t cube,
    int max_depth,
    uint8_t* ptable,
    struct c_index_cclass_sym* cclass,
    int* solution
);

#endif /* __SOLVERS_H_ */
