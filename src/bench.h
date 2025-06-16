#ifndef BENCH_H
#define BENCH_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "tables.h"
#include "core/cube.h"
#include "index.h"
#include "scrambler.h"
#include "solver.h"
#include "print_cube.h"

#define NUM_MOVES 1000000UL

void bench();
void bench_solver();

#endif /* BENCH_H */
