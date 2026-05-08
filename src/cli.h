#ifndef CLI_H
#define CLI_H

#include <argp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "core/cube.h"
#include "core/move.h"

#include "scrambler.h"
#include "solutions.h"

#define MAX_STEPS 32
#define BUF_SIZE 4096

/* Prints the cube in a human-readable format. */
void cube_print_cube(cube_t* cube);

/* Prints solutions in a human-readable format.
 * solutions: array of size: sizeof(int) * 20 * num_sols*/
void cube_print_solutions(int* solutions, int num_sols, int verbose);

/* Prints a set of solutions in a human-readable format. */
void cube_print_solution_set(SolutionSet* solution_set, int verbose);

/* Prints a set of solutions used in pipeline mode in a human-readable format. */
void cube_print_pipelinesolution_set(PipelineSolutionSet* p, int verbose);
void cube_print_pipelinesolution(PipelineSolution* ps, int verbose);

/* need to rewrite this function someday */
int* parse_move_string(size_t* out_length, const char* move_string);

struct step {
    char *name;
    int max_depth;
};

/* Used by main to communicate with parse_opt. */
struct arguments {
    char* format;
    int   verbose;
    int   gen;
    int   number_of_solutions;
    int   depth_limit;

    struct step steps[MAX_STEPS];
    int step_count;
};


error_t parse_opt(int key, char* arg, struct argp_state* state);

void set_default_values_arguments(struct arguments* arguments);


/* main should call these functions when we pass cli arguments */
void cli_gen();
int cli_solver_prepare(struct arguments arguments, solving_step** steps);
void cli_solver_solving_loop(struct arguments arguments, solving_step** steps);
void cli_solver_cleanup(struct arguments arguments, solving_step** steps);


#endif /* CLI_H */
