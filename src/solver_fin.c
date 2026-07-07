#include "solver.h"
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
    int premoves_sequence_id;
    ptable_data_t* ptable_data;
    shared_data_t* shared_data;
    struct solver_stats* stats;  // local stats per task
} TreeSearchTask;


/* Helper */

Solution solution_merge_normal_and_inverse(Solution* temp_solution, Solution* temp_solution_inv) {
    Solution out = solution_copy(temp_solution);
    for (int move_idx = temp_solution_inv->length - 1; move_idx >= 0; move_idx--)
    {
        int move_inv = temp_solution_inv->moves[move_idx];
        int move     = get_inv_move(move_inv);

        solution_append(&out, move);
    }

    return out;
}

/* Searching */

static bool TreeSearch_fin(cube_t*              cube,
                           ptable_data_t*       ptable_data,
                           struct search_data   s_data,
                           shared_data_t*       shared_data,
                           struct solver_stats* stats) {
    stats->no_nodes_visited++;
    int  remaining_moves = s_data.remaining_moves;
    int  prev_move       = s_data.prev_move;
    int  prev_move_inv   = s_data.prev_move_inv;
    bool is_inv          = s_data.is_inv;
    int  max_num_sols    = s_data.max_num_sols;
    bool enable_niss     = s_data.enable_niss;

    Solution*    temp_solution = s_data.temp_solution;
    Solution*    temp_solution_inv = s_data.temp_solution_inv;
    SolutionSet* solution_set  = s_data.solution_set;

    if (s_data.stop_flag && *s_data.stop_flag) {
        return true;
    }

    if (remaining_moves == 0)
    {
        if (cube_state_is_solved(cube))
        {
             Solution sol_out = solution_merge_normal_and_inverse(temp_solution, temp_solution_inv);

             pthread_mutex_lock(&shared_data->mutex);
             solutionset_add_copy(solution_set, &sol_out);
             solution_free(&sol_out);

             shared_data->number_of_solutions_found++;
             bool done = (shared_data->number_of_solutions_found == max_num_sols);
             if (done) {
                 *(s_data.stop_flag) = true;
             }
             pthread_mutex_unlock(&shared_data->mutex);

             stats->num_sol_found++;

             return done;
        }
        return false;
    }

    uint64_t      p1      = ptable_data->cube_to_index_func(cube, UD);
    const uint8_t pval_UD = ptable_data->read_value_ptable_func(p1, ptable_data->ptable);

    if (pval_UD > remaining_moves)
    {
        stats->no_nodes_pruned++;
        return false;
    }
    uint64_t      p2      = ptable_data->cube_to_index_func(cube, LR);
    const uint8_t pval_LR = ptable_data->read_value_ptable_func(p2, ptable_data->ptable);

    if (pval_LR > remaining_moves)
    {
        stats->no_nodes_pruned++;
        return false;
    }
    uint64_t      p3      = ptable_data->cube_to_index_func(cube, FB);
    const uint8_t pval_FB = ptable_data->read_value_ptable_func(p3, ptable_data->ptable);

    if (pval_FB > remaining_moves)
    {
        stats->no_nodes_pruned++;
        return false;
    }
    if ((pval_UD == remaining_moves) && (pval_UD == pval_LR) && (pval_LR == pval_FB))
    {
        stats->no_nodes_pruned++;
        return false;
    }

    // we only look up inverse if we did not prune on normal
    cube_t inv = cube_operation_inverse(*cube);
    stats->no_inverse_computations++;

    uint64_t      p1_inv      = ptable_data->cube_to_index_func(&inv, UD);
    const uint8_t pval_UD_inv = ptable_data->read_value_ptable_func(p1_inv, ptable_data->ptable);


    if (pval_UD_inv > remaining_moves)
    {
        stats->no_nodes_pruned++;
        stats->no_nodes_pruned_inv++;
        return false;
    }

    uint64_t      p2_inv      = ptable_data->cube_to_index_func(&inv, LR);
    const uint8_t pval_LR_inv = ptable_data->read_value_ptable_func(p2_inv, ptable_data->ptable);

    if (pval_LR_inv > remaining_moves)
    {
        stats->no_nodes_pruned++;
        stats->no_nodes_pruned_inv++;
        return false;
    }

    uint64_t      p3_inv      = ptable_data->cube_to_index_func(&inv, FB);
    const uint8_t pval_FB_inv = ptable_data->read_value_ptable_func(p3_inv, ptable_data->ptable);

    if (pval_FB_inv > remaining_moves)
    {
        stats->no_nodes_pruned++;
        stats->no_nodes_pruned_inv++;
        return false;
    }

    if ((pval_UD_inv == remaining_moves) && (pval_UD_inv == pval_LR_inv)
        && (pval_LR_inv == pval_FB_inv))
    {
        stats->no_nodes_pruned++;
        stats->no_nodes_pruned_inv++;
        return false;
    }

    // find out if we niss or not!
    bool _niss = false;
    if (enable_niss)
    {
        int num_axis_to_check_normal  = 0;
        int num_axis_to_check_inverse = 0;

        if (pval_UD < remaining_moves)
            num_axis_to_check_inverse++;
        if (pval_LR < remaining_moves)
            num_axis_to_check_inverse++;
        if (pval_FB < remaining_moves)
            num_axis_to_check_inverse++;
        if (pval_UD_inv < remaining_moves)
            num_axis_to_check_normal++;
        if (pval_LR_inv < remaining_moves)
            num_axis_to_check_normal++;
        if (pval_FB_inv < remaining_moves)
            num_axis_to_check_normal++;

        _niss = num_axis_to_check_normal > num_axis_to_check_inverse;
    }
    // niss
    if (_niss)
    {
        swap_cubes(cube, &inv);
        is_inv = is_inv ? false : true;
        stats->no_nisses++;
    }

    // we extract the move_mask for this node.
    uint32_t mm = move_mask[is_inv ? prev_move_inv : prev_move];

    // nissing magic
    if (remaining_moves == (_niss ? pval_UD : pval_UD_inv))
        mm &= ~((uint32_t) 63 << 0);
    if (remaining_moves == (_niss ? pval_LR : pval_LR_inv))
        mm &= ~((uint32_t) 63 << 6);
    if (remaining_moves == (_niss ? pval_FB : pval_FB_inv))
        mm &= ~((uint32_t) 63 << 12);

    for (int move = 0; move < NMOVES; move++)
    {
        // check if we actually need to do move.
        if (!(mm & (1 << move)))
            continue;

        cube_move_apply_move(cube, move);

        if (!is_inv)
        {
            solution_append(temp_solution, move);
        }
        else
        {
            solution_append(temp_solution_inv, move);
        }

        struct search_data s_data_next = {.remaining_moves   = remaining_moves - 1,
                                          .prev_move         = !is_inv ? move : prev_move,
                                          .prev_move_inv     = is_inv ? move : prev_move_inv,
                                          .is_inv            = is_inv,
                                          .max_num_sols      = max_num_sols,
                                          .enable_niss       = enable_niss,
                                          .temp_solution     = temp_solution,
                                          .temp_solution_inv = temp_solution_inv,
                                          .solution_set      = solution_set,
                                          .stop_flag         = s_data.stop_flag};
        bool found = TreeSearch_fin(cube, ptable_data, s_data_next, shared_data, stats);

        cube_move_apply_move(cube, get_inv_move(move));

        if (!is_inv)
        {
            solution_pop(temp_solution);
        }
        else
        {
            solution_pop(temp_solution_inv);
        }

        if (found)
        {
            return true;
        }
    }

    if (_niss)
    {
        swap_cubes(cube, &inv);
    }
    return false;
}




void start_treesearch_task(int thread_id, void* task, void* local){
    TreeSearchTask* treesearch_task = (TreeSearchTask*)task;

    cube_t cube_task           = treesearch_task->cube;
    int* premove_sequence      = treesearch_task->premoves;
    int premove_sequence_id    = treesearch_task->premoves_sequence_id;
    int depth                  = treesearch_task->depth;
    int max_num_sols           = treesearch_task->max_num_sols;
    bool niss                  = treesearch_task->niss;
    shared_data_t* shared_data = treesearch_task->shared_data;
    ptable_data_t* ptable_data = treesearch_task->ptable_data;
    struct solver_stats* stats = treesearch_task->stats;

    Solution temp_solution, temp_solution_inv;
    solution_init(&temp_solution);
    solution_init(&temp_solution_inv);
    
    /* remember we have to start of by applying the premove sequence */
    for (int j = 0; j < INITIAL_MOVE_SEQUENCE_LENGTH; j++){
        cube_move_apply_move(&cube_task, (premove_sequence[j]));
        solution_append(&temp_solution, premove_sequence[j]);
    }

    struct search_data s_data = {.remaining_moves   = depth - INITIAL_MOVE_SEQUENCE_LENGTH,
                                 .prev_move         = premove_sequence[INITIAL_MOVE_SEQUENCE_LENGTH - 1],
                                 .prev_move_inv     = NULLMOVE,
                                 .is_inv            = false,
                                 .max_num_sols      = max_num_sols,
                                 .enable_niss       = niss,
                                 .temp_solution     = &temp_solution,
                                 .temp_solution_inv = &temp_solution_inv,
                                 .solution_set      = shared_data->solution_set,
                                 .stop_flag         = &shared_data->stop_flag};

    bool stop_search = TreeSearch_fin(&cube_task, ptable_data, s_data, shared_data, stats);

    solution_free(&temp_solution);
    solution_free(&temp_solution_inv);


    // 58206 : 47525 *~= sqrt(3) : sqrt(2)
    uint64_t f = (premove_sequence[INITIAL_MOVE_SEQUENCE_LENGTH - 1] / 9 == 0)
        ? 47525
        : 58206;
    shared_data->move_sequence_cost[premove_sequence_id].cost = stats->no_nodes_visited * f;
}

void IDA_fin(cube_t               cube,
             ptable_data_t*       ptable_data,
             struct solver_stats* stats,
             SolutionSet*         solution_set,
             int                  max_num_sols,
             int                  verbose,
             bool                 niss,
             int                  number_of_threads,
             int                  depth_limit) {
    // create a thread pool
    ThreadPool* tp = thread_pool_create(number_of_threads, 0);

    bool stop_search = false;
    if (verbose == 1)
    {
        fprintf(stderr, "Depth: ");
    }

    shared_data_t shared_data = {
        .move_sequence_cost = ms,
        .solution_set = solution_set,
        .stop_flag = false,
        .mutex = PTHREAD_MUTEX_INITIALIZER,
        .number_of_solutions_found = 0,
        .solver_stats = stats,
    };

    // iterative deepening until stop_search is set to true.
    int depth = 0;
    while (depth <= ((depth_limit == -1) ? 1024 : depth_limit))
    {
        stats->depth = depth;
        if (verbose == 1)
        {
            fprintf(stderr, "%i ", depth);
        }

        Solution temp_solution, temp_solution_inv;
        solution_init(&temp_solution);
        solution_init(&temp_solution_inv);

        if (depth < 15) {
            struct search_data s_data = {.remaining_moves = depth,
                                         .prev_move       = NULLMOVE,
                                         .prev_move_inv   = NULLMOVE,
                                         .is_inv          = false,
                                         .max_num_sols    = max_num_sols,
                                         .enable_niss     = niss,
                                         .temp_solution   = &temp_solution,
                                         .temp_solution_inv = &temp_solution_inv,
                                         .solution_set    = solution_set,
                                         .stop_flag       = &shared_data.stop_flag,
            };
            stop_search = TreeSearch_fin(&cube, ptable_data, s_data, &shared_data, stats);
        }
        else {

            // we want to use the 4 move sequences as starting point
            // and sort them based on number of nodes that got pruned
            // create tasks for each of them and give them to the thread pool
            int number_of_tasks = NUMBER_OF_4_MOVE_SEQUENCES;
            TreeSearchTask* tasks = malloc(sizeof(TreeSearchTask) * number_of_tasks);

            struct solver_stats* stats_tasks = malloc(sizeof(struct solver_stats)*number_of_tasks);
            for (size_t i = 0; i < NUMBER_OF_4_MOVE_SEQUENCES; ++i){
                init_stats(&stats_tasks[i], max_num_sols);
                tasks[i] = (TreeSearchTask){
                    .cube = cube,
                    .depth = depth,
                    .max_num_sols = max_num_sols,
                    .niss = niss,
                    .premoves = ms[i].moves,
                    .premoves_sequence_id = i,
                    .ptable_data = ptable_data,
                    .shared_data = &shared_data,
                    .stats = &stats_tasks[i],
                };
            }

            void** task_ptrs = malloc(sizeof(void*) * number_of_tasks);
            for (size_t i = 0; i < number_of_tasks; i++) {
                task_ptrs[i] = &tasks[i];
            }

            thread_pool_execute(tp, task_ptrs, number_of_tasks, &start_treesearch_task);

            free(task_ptrs);

            if (shared_data.stop_flag){
                stop_search = true;
            }

            // this is the end of this depth, so we sort
            // based on the number of nodes visited by that branch (cost).
            // branch with higher number of nodes should be searched first.
            qsort(ms, NUMBER_OF_4_MOVE_SEQUENCES, sizeof(struct move_sequence_cost), compare_move_sequence_cost);
            if (verbose == 1){
                fprintf(stderr, "(sort) ");
            }

            // Also, we need to copy over (or rather append)
            // the stats collected by each of the tasks to the main shared stats struct
            for (size_t i = 0; i < NUMBER_OF_4_MOVE_SEQUENCES; ++i){
                append_stats(&stats_tasks[i], stats);
            }

            // local_data_t local_datas[NUMBER_OF_4_MOVE_SEQUENCES];

            free(stats_tasks);
            free(tasks);
        }

        solution_free(&temp_solution);
        solution_free(&temp_solution_inv);

        if (stop_search)
        {
            break;
        }

        depth++;
    }

    thread_pool_destroy(tp);

    if (verbose == 1)
    {
        fprintf(stderr, "\n");
    }
}
