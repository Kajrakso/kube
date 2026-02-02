#ifndef SOLVER_PIPELINE_H
#define SOLVER_PIPELINE_H

#include <stdlib.h>
#include <time.h>

#include "solver.h"
#include "cli.h"
#include "scrambler.h"
#include "env.h"

#include "tables_ptable_data.h"
#include "solver_steps.h"


void solver_pipeline(cube_t c, int* solutions, struct arguments arguments, solving_step** steps);
void solver_beam_search(cube_t c, int* solutions, struct arguments arguments, solving_step** steps);


#endif  /* SOLVER_PIPELINE_H */
