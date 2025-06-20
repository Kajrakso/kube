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
#include "cli.h"

#define NUM_MOVES 1000000UL
#define NUM_SCRAMBLES 50

void bench();
void bench_solver();

void bench_solver_16_movers();

#endif /* BENCH_H */
