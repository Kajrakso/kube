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

#include "solutions.h"
#include "thread_pool.h"


struct solver_stats {
    int           depth;
    int           num_sol_found;
    unsigned long no_nisses;
    unsigned long no_nodes_visited;
    unsigned long no_nodes_pruned;
    unsigned long no_nodes_pruned_inv;
    unsigned long no_inverse_computations;
    int           no_equal_pvals_normal;
    int           no_equal_pvals_inverse;
};

void init_stats(struct solver_stats* stats, int max_num_sols);
void print_stats(struct solver_stats* stats);
void append_stats(struct solver_stats* src, struct solver_stats* dest);

/* struct that holds information we need during the search */
struct search_data {
    int  remaining_moves;
    int  prev_move;
    int  prev_move_inv;
    bool is_inv;
    int  max_num_sols;
    bool enable_niss;
    // int max_num_nomves; // not implemented yet

    // we use a temporary solution to store the current search
    // and when we find a solution we add it to the solution set.
    // when we also search on inverse, we have to keep track of both paths.
    Solution*    temp_solution;
    Solution*    temp_solution_inv;
    SolutionSet* solution_set;

    bool* stop_flag;
};

/* structs to keep track of "shared" and "per-task" data in use during multithreaded search */

typedef struct {
    struct move_sequence_cost* move_sequence_cost;
    SolutionSet* solution_set;
    bool stop_flag;
    pthread_mutex_t mutex;
    int number_of_solutions_found;
    struct solver_stats* solver_stats;  // global stats for the entire search
} shared_data_t;


typedef struct {
    cube_t cube;
    int depth; 
    int max_num_sols; 
    bool niss; 
    int* premoves;
    size_t premoves_sequence_id;
    ptable_data_t* ptable_data;
    shared_data_t* shared_data;
    struct solver_stats* stats;  // local stats per task
} TreeSearchTask;

bool cube_solvers_solve_cube(cube_t        cube,
                             SolutionSet*  solution_set,
                             int           number_of_solutions,
                             int           max_depth,
                             int           verbose,
                             int           number_of_threads,
                             solving_step* ss);

void IDA_fin(cube_t               cube,
             ptable_data_t*       ptable_data,
             struct solver_stats* stats,
             SolutionSet*         solution_set,
             int                  max_num_sols,
             int                  verbose,
             bool                 niss,
             int                  number_of_threads,
             int                  max_depth);

#endif /* SOLVER_H */
