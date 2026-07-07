#ifndef PARALLEL_SEARCH_H
#define PARALLEL_SEARCH_H

#include "solutions.h"

#include <pthread.h>

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

#endif /* PARALLEL_SEARCH_H */
