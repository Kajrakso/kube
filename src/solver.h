#ifndef SOLVER_H
#define SOLVER_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tables.h"
#include "core/cube.h"
#include "core/cube_state.h"
#include "index.h"
#include "solver_steps.h"

static const int GODS_NO     = 20;

struct solver_stats {
    int           depth;
    int*          solution;
    int*          solution_inv;
    int           num_sol_found;
    unsigned long no_nisses;
    unsigned long no_nodes_visited;
    unsigned long no_nodes_pruned;
    unsigned long no_nodes_pruned_inv;
    unsigned long no_inverse_computations;
    int           no_equal_pvals_normal;
    int           no_equal_pvals_inverse;

    int* solutions;
};

void init_stats(struct solver_stats* stats, int max_num_sols);
void print_stats(struct solver_stats* stats);
void save_solution_to_solutions(struct solver_stats* s);

/* struct that holds information we need during the search */
struct search_data {
    int remaining_moves;
    int prev_move;
    int prev_move_inv;
    bool is_inv;
    int max_num_sols;
    bool enable_niss;
    // int max_num_nomves; // not implemented yet
};


void IDA_fin(cube_t cube,
        ptable_data_t* ptable_data,
        struct solver_stats* stats, int max_num_sols, int verbose, bool niss);

// void IDA(
//     cube_t cube,
//     ptable_data_t ptable_data,
//     solving_step ss,
//     struct solver_stats* stats,
//     int max_num_sols,
//     int verbose,
//     bool niss
// );

bool cube_solvers_solve_cube(cube_t cube, int* solutions, int number_of_solutions, int verbose, solving_step* ss);

#endif /* SOLVER_H */
