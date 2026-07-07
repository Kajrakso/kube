#include "solver.h"
#include "parallel_search.h"

/* Prune */

static inline bool try_prune(
    cube_t* cube,
    ptable_data_t* ptable_data,
    int axis,
    int remaining_moves,
    struct solver_stats* stats,
    uint8_t* out) {

    uint64_t index = ptable_data->cube_to_index_func(cube, axis);
    uint8_t pval = ptable_data->read_value_ptable_func(index, ptable_data->ptable);
    *out = pval;
    if (pval > remaining_moves)
    {
        stats->no_nodes_pruned++;
        return true;
    }
    return false;
}

/* Other "helper" function in use during search */

static inline bool decide_niss(
    uint8_t pval_UD,
    uint8_t pval_LR,
    uint8_t pval_FB,
    uint8_t pval_UD_inv,
    uint8_t pval_LR_inv,
    uint8_t pval_FB_inv,
    int remaining_moves
){
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

    return num_axis_to_check_normal > num_axis_to_check_inverse;
}


// Build the move mask. niss_magic adjusts for the "nissing magic" optimization.
static inline uint32_t build_move_mask(
    int prev_move, int prev_move_inv, bool is_inv,
    bool niss,
    uint8_t pval_UD,
    uint8_t pval_LR,
    uint8_t pval_FB,
    uint8_t pval_UD_inv,
    uint8_t pval_LR_inv,
    uint8_t pval_FB_inv,
    int remaining_moves){

    // we extract the move_mask for this node.
    uint32_t mm = move_mask[is_inv ? prev_move_inv : prev_move];

    // nissing magic
    if (remaining_moves == (niss ? pval_UD : pval_UD_inv))
        mm &= ~((uint32_t) 63 << 0);
    if (remaining_moves == (niss ? pval_LR : pval_LR_inv))
        mm &= ~((uint32_t) 63 << 6);
    if (remaining_moves == (niss ? pval_FB : pval_FB_inv))
        mm &= ~((uint32_t) 63 << 12);

    return mm;
}

static inline bool move_in_move_mask(uint32_t mm, int m) {
    return (mm & (1 << m));
}


static bool solution_submit(
    Solution* sol,
    struct search_data* s_data,
    shared_data_t* shared_data,
    struct solver_stats* stats,
    int max_num_sols
){
    pthread_mutex_lock(&shared_data->mutex);
    solutionset_add_copy(s_data->solution_set, sol);
    solution_free(sol);

    shared_data->number_of_solutions_found++;
    bool done = (shared_data->number_of_solutions_found == max_num_sols);
    if (done) {
     *(s_data->stop_flag) = true;
    }
    pthread_mutex_unlock(&shared_data->mutex);

    stats->num_sol_found++;
    return done;
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

    Solution*    temp_solution     = s_data.temp_solution;
    Solution*    temp_solution_inv = s_data.temp_solution_inv;
    SolutionSet* solution_set      = s_data.solution_set;

    if (s_data.stop_flag && *s_data.stop_flag) {
        return true;
    }

    if (remaining_moves == 0)
    {
        if (!cube_state_is_solved(cube)) return false;

         Solution sol = solution_merge_normal_and_inverse(temp_solution, temp_solution_inv);
         bool done = solution_submit(&sol, &s_data, shared_data, stats, max_num_sols);
         return done;
    }
    
    uint8_t pval_UD, pval_LR, pval_FB;
    if (try_prune(cube, ptable_data, UD, remaining_moves, stats, &pval_UD)) return false;
    if (try_prune(cube, ptable_data, LR, remaining_moves, stats, &pval_LR)) return false;
    if (try_prune(cube, ptable_data, FB, remaining_moves, stats, &pval_FB)) return false;

    if ((pval_UD == remaining_moves) && (pval_UD == pval_LR) && (pval_LR == pval_FB))
    {
        stats->no_nodes_pruned++;
        return false;
    }

    // we only look up inverse if we did not prune on normal
    cube_t cube_inv = cube_operation_inverse(*cube);
    stats->no_inverse_computations++;

    uint8_t pval_UD_inv, pval_LR_inv, pval_FB_inv;
    if (try_prune(&cube_inv, ptable_data, UD, remaining_moves, stats, &pval_UD_inv)) return false;
    if (try_prune(&cube_inv, ptable_data, LR, remaining_moves, stats, &pval_LR_inv)) return false;
    if (try_prune(&cube_inv, ptable_data, FB, remaining_moves, stats, &pval_FB_inv)) return false;

    if ((pval_UD_inv == remaining_moves) && (pval_UD_inv == pval_LR_inv)
        && (pval_LR_inv == pval_FB_inv))
    {
        stats->no_nodes_pruned++;
        stats->no_nodes_pruned_inv++;
        return false;
    }
    
    // find out if we niss or not!
    bool _niss = enable_niss ? decide_niss(pval_UD, pval_LR, pval_FB, pval_UD_inv, pval_LR_inv, pval_FB_inv, remaining_moves) : false;

    if (_niss)
    {
        swap_cubes(cube, &cube_inv);
        is_inv = is_inv ? false : true;
        stats->no_nisses++;
    }

    uint32_t mm = build_move_mask(
        prev_move, prev_move_inv, is_inv, _niss,
        pval_UD,
        pval_LR,
        pval_FB,
        pval_UD_inv,
        pval_LR_inv,
        pval_FB_inv,
        remaining_moves
    );

    for (int move = 0; move < NMOVES; move++)
    {
        if (!move_in_move_mask(mm, move)) {
            continue;
        }
        

        cube_move_apply_move(cube, move);
        solution_append(is_inv ? temp_solution_inv : temp_solution, move);
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
        solution_pop(is_inv ? temp_solution_inv : temp_solution);

        if (found)
        {
            return true;
        }
    }

    if (_niss)
    {
        swap_cubes(cube, &cube_inv);
    }
    return false;
}




void start_treesearch_task(int thread_id, void* task, void* local){
    TreeSearchTask* treesearch_task = (TreeSearchTask*)task;

    cube_t cube_task           = treesearch_task->cube;
    int* premove_sequence      = treesearch_task->premoves;
    size_t premove_sequence_id = treesearch_task->premoves_sequence_id;
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

    TreeSearch_fin(&cube_task, ptable_data, s_data, shared_data, stats);

    solution_free(&temp_solution);
    solution_free(&temp_solution_inv);

    // 58206 : 47525 *~= sqrt(3) : sqrt(2)
    uint64_t f = (premove_sequence[INITIAL_MOVE_SEQUENCE_LENGTH - 1] / 9 == 0)
        ? 47525
        : 58206;
    shared_data->move_sequence_cost[premove_sequence_id].cost = stats->no_nodes_visited * f;
}


TreeSearchTask* create_tasks(size_t number_of_tasks, cube_t cube, int depth, int max_num_sols, bool niss,
                          ptable_data_t* ptable_data, shared_data_t* shared_data, struct solver_stats* stats_tasks){
    TreeSearchTask* tasks = malloc(sizeof(TreeSearchTask) * number_of_tasks);

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
            .shared_data = shared_data,
            .stats = &stats_tasks[i],
        };
    }
    
    return tasks;
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
            if (shared_data.stop_flag){
                stop_search = true;
            }
        }
        else {

            // we want to use the 4 move sequences as starting point
            // and sort them based on number of nodes that got pruned
            // create tasks for each of them and give them to the thread pool
            const size_t number_of_tasks = NUMBER_OF_4_MOVE_SEQUENCES;
            struct solver_stats* stats_tasks = malloc(sizeof(struct solver_stats)*number_of_tasks);
            TreeSearchTask* tasks = create_tasks(number_of_tasks, cube, depth, max_num_sols, niss,
                          ptable_data, &shared_data, stats_tasks);
            
            void** task_ptrs = malloc(sizeof(void*) * number_of_tasks);
            for (size_t i = 0; i < number_of_tasks; i++) {
                task_ptrs[i] = &tasks[i];
            }
            thread_pool_execute(tp, task_ptrs, number_of_tasks, &start_treesearch_task);

            if (shared_data.stop_flag){
                stop_search = true;
            }

            // this is the end of this depth, so we sort
            // based on the number of nodes visited by that branch (cost).
            // branch with higher number of nodes should be searched first.
            qsort(ms, NUMBER_OF_4_MOVE_SEQUENCES, sizeof(struct move_sequence_cost), compare_move_sequence_cost);

            // Also, we need to copy over (or rather append)
            // the stats collected by each of the tasks to the main shared stats struct
            for (size_t i = 0; i < NUMBER_OF_4_MOVE_SEQUENCES; ++i){
                append_stats(&stats_tasks[i], stats);
            }

            free(stats_tasks);
            free(tasks);
            free(task_ptrs);
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
