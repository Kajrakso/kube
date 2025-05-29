#ifndef __PRINT_CUBE_H_
#define __PRINT_CUBE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/cli/print_cube.h"
#include "core/_cube.h"
#include "core/_move.h"

// need to rewrite this function someday
int* parse_move_string(size_t* out_length, const char* move_string);

void cube_print_solutions(int* solutions, int num_sols);

#endif /* __PRINT_CUBE_H_ */
