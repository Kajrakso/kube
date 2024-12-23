#ifndef __PRINT_CUBE_H_
#define __PRINT_CUBE_H_

#include "../include/cli/print_cube.h"
#include "_cube.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// need to rewrite this function someday
int* parse_move_string(size_t* out_length, const char* move_string);

#endif /* __PRINT_CUBE_H_ */