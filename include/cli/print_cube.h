#ifndef _PRINT_CUBE_H_
#define _PRINT_CUBE_H_

#include "../core/cube.h"
#include "../core/move.h"

/* Prints the cube in a human-readable format. */
void cube_print_cube(cube_t* cube);

/* Prints solutions in a human-readable format.
 * solutions: array of size: sizeof(int) * 20 * num_sols*/
void cube_print_solutions(int* solutions, int num_sols);

#endif /* _PRINT_CUBE_H_ */
