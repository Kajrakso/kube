#ifndef CLI_H
#define CLI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/cube.h"
#include "core/move.h"

/* Prints the cube in a human-readable format. */
void cube_print_cube(cube_t* cube);

/* Prints solutions in a human-readable format.
 * solutions: array of size: sizeof(int) * 20 * num_sols*/
void cube_print_solutions(int* solutions, int num_sols, int verbose);

/* need to rewrite this function someday */
int* parse_move_string(size_t* out_length, const char* move_string);

#endif /* CLI_H */
