#ifndef _SOLVERS_H_
#define _SOLVERS_H_

#include "../core/cube.h"
#include "../core/move.h"
#include "../core/tables.h"

/* Solve a cube. */
bool cube_solvers_solve_cube(cube_t cube, int* solution, int solution_length);

#endif // _SOLVERS_H_
