#ifndef _PRINT_CUBE_H_
#define _PRINT_CUBE_H_

#include "../core/move.h"
#include "../core/cube.h"

/* Prints the cube in a human-readable format. */
void cube_print_cube(cube_t* cube);

/* Prints the solution. */
void cube_print_solution_string(int* solution, int solution_length);

#endif /* _PRINT_CUBE_H_ */