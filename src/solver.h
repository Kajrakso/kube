#ifndef SOLVER_H
#define SOLVER_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tables.h"
#include "core/cube.h"
#include "core/cube_state.h"
#include "index.h"

bool cube_solvers_solve_cube(cube_t cube, int* solutions, int number_of_solutions, int verbose);

#endif /* SOLVER_H */
