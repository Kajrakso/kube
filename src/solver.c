#include "solver.h"

static const int GODS_NO     = 20;
static bool      enable_niss = true;

// stats (temp)

struct stats {
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

void init_stats(struct stats* stats, int max_num_sols) {
    stats->depth                   = 0;
    stats->no_nodes_visited        = 0;
    stats->no_nodes_pruned         = 0;
    stats->no_nodes_pruned_inv     = 0;
    stats->no_nisses               = 0;
    stats->no_inverse_computations = 0;
    stats->no_equal_pvals_normal   = 0;
    stats->no_equal_pvals_inverse  = 0;

    stats->num_sol_found = 0;
    stats->solution      = malloc(sizeof(int) * 20);
    for (int i = 0; i < 20; i++)
    {
        stats->solution[i] = -1;
    }

    stats->solution_inv = malloc(sizeof(int) * 20);
    for (int i = 0; i < 20; i++)
    {
        stats->solution_inv[i] = -1;
    }

    stats->solutions = malloc(sizeof(int) * 20 * max_num_sols);
    for (int i = 0; i < 20 * max_num_sols; i++)
    {
        stats->solutions[i] = -1;
    }
}

static void print_stats(struct stats* stats) {
    if (stats->no_nodes_visited == 0)
    {
        fprintf(stderr, "No nodes visited\n");
    }
    else
    {
        fprintf(stderr, "\n");
        fprintf(stderr, "Some statistics:\n");
        fprintf(stderr, "max depth: %i\n", stats->depth);
        fprintf(stderr, "sols found: %i\n", stats->num_sol_found);
        fprintf(stderr, "\n");
        fprintf(stderr, "nodes visited: %lu\n", stats->no_nodes_visited);
        fprintf(stderr, "nodes pruned: %lu (%.2f%%)\n", stats->no_nodes_pruned,
                100 * (float) stats->no_nodes_pruned / (float) stats->no_nodes_visited);
        fprintf(stderr, "nodes pruned inv: %lu (%.2f%%)\n", stats->no_nodes_pruned_inv,
                100 * (float) stats->no_nodes_pruned_inv / (float) stats->no_nodes_visited);
        fprintf(stderr, "inverse computations: %lu (%.2f%%)\n", stats->no_inverse_computations,
                100 * (float) stats->no_inverse_computations / (float) stats->no_nodes_visited);
        fprintf(stderr, "nisses: %lu (%.2f%%)\n", stats->no_nisses,
                100 * (float) stats->no_nisses / (float) stats->no_nodes_visited);
        fprintf(stderr, "\n");
    }
}

/* struct that holds information we need during the search */
struct search_data {
    int remaining_moves;
    int prev_move;
    int prev_move_inv;
    bool is_inv;
    int max_num_sols;
    // int max_num_nomves; // not implemented yet
};


void save_solution_to_solutions(struct stats* s) {
    int i = 0;
    for (int m = 0; m < GODS_NO; m++)
    {
        if (0 <= s->solution[m] && s->solution[m] < NMOVES)
        {
            s->solutions[s->num_sol_found * GODS_NO + i++] = s->solution[m];
        }
    }
    for (int m = GODS_NO - 1; m >= 0; m--)
    {
        if (0 <= s->solution_inv[m] && s->solution_inv[m] < 18)
        {
            s->solutions[s->num_sol_found * GODS_NO + i++] = get_inv_move(s->solution_inv[m]);
        }
    }
}

// work in progress

static bool TreeSearch(cube_t*       cube,
                       ptable_data_t ptable_data,
                       struct search_data s_data,
                       struct stats* stats
    ) {
    stats->no_nodes_visited++;
    int remaining_moves = s_data.remaining_moves;
    int prev_move = s_data.prev_move;
    int prev_move_inv = s_data.prev_move_inv;
    bool is_inv = s_data.is_inv;
    int max_num_sols = s_data.max_num_sols;




    if (remaining_moves == 0)
    {
        if (cube_state_is_solved(cube))
        {
            save_solution_to_solutions(stats);
            stats->num_sol_found++;

            // keep searching if we
            // have not found enough
            // solutions
            return stats->num_sol_found == max_num_sols;
        }
        else
        {
            return false;
        }
    }

    uint64_t      p1      = ptable_data.cube_to_index_func(cube, UD);
    const uint8_t pval_UD = ptable_data.read_value_ptable_func(p1, ptable_data.ptable);

    if (pval_UD > remaining_moves)
    {
        stats->no_nodes_pruned++;
        return false;
    }
    uint64_t      p2      = ptable_data.cube_to_index_func(cube, LR);
    const uint8_t pval_LR = ptable_data.read_value_ptable_func(p2, ptable_data.ptable);

    if (pval_LR > remaining_moves)
    {
        stats->no_nodes_pruned++;
        return false;
    }
    uint64_t      p3      = ptable_data.cube_to_index_func(cube, FB);
    const uint8_t pval_FB = ptable_data.read_value_ptable_func(p3, ptable_data.ptable);

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

    uint64_t      p1_inv      = ptable_data.cube_to_index_func(&inv, UD);
    const uint8_t pval_UD_inv = ptable_data.read_value_ptable_func(p1_inv, ptable_data.ptable);


    if (pval_UD_inv > remaining_moves)
    {
        stats->no_nodes_pruned++;
        stats->no_nodes_pruned_inv++;
        return false;
    }

    uint64_t      p2_inv      = ptable_data.cube_to_index_func(&inv, LR);
    const uint8_t pval_LR_inv = ptable_data.read_value_ptable_func(p2_inv, ptable_data.ptable);

    if (pval_LR_inv > remaining_moves)
    {
        stats->no_nodes_pruned++;
        stats->no_nodes_pruned_inv++;
        return false;
    }

    uint64_t      p3_inv      = ptable_data.cube_to_index_func(&inv, FB);
    const uint8_t pval_FB_inv = ptable_data.read_value_ptable_func(p3_inv, ptable_data.ptable);

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
            stats->solution[GODS_NO - remaining_moves] = move;
        else
            stats->solution_inv[GODS_NO - remaining_moves] = move;

        struct search_data s_data_next = {
            .remaining_moves = remaining_moves - 1,
            .prev_move = !is_inv ? move : prev_move,
            .prev_move_inv = is_inv ? move : prev_move_inv,
            .is_inv = is_inv,
            .max_num_sols = max_num_sols
        };
        bool found = TreeSearch(cube, ptable_data, s_data_next, stats);

        cube_move_apply_move(cube, get_inv_move(move));

        if (!is_inv)
            stats->solution[GODS_NO - remaining_moves] = -1;
        else
            stats->solution_inv[GODS_NO - remaining_moves] = -1;

        if (found)
            return true;
    }

    if (_niss)
        swap_cubes(cube, &inv);
    return false;
}

void IDA(cube_t cube,
        ptable_data_t ptable_data,
        struct stats* stats, int max_num_sols, int verbose) {
    bool stop_search = false;
    if (verbose == 1)
    {
        fprintf(stderr, "Depth: ");
    }

    // in principle we could search beyond 20
    // moves if we want to find multiple solutions,
    // but I doubt this is very useful
    for (int depth = 0; depth <= GODS_NO; depth++)
    {
        stats->depth = depth;
        if (verbose == 1)
        {
            fprintf(stderr, "%i ", depth);
        }
        

        struct search_data s_data = {
            .remaining_moves = depth,
            .prev_move = 18,
            .prev_move_inv = 18,
            .is_inv = false,
            .max_num_sols = max_num_sols,
        };
        stop_search = TreeSearch(&cube, ptable_data, s_data, stats);

        if (stop_search)
        {
            if (verbose == 1)
            {
                fprintf(stderr, "\n");
            }
            return;
        }
    }
    if (verbose == 1)
    {
        fprintf(stderr, "\n");
    }
}

/* public */

bool cube_solvers_solve_cube(cube_t cube, int* solutions, int number_of_solutions, int verbose, ptable_data_t ptable_data) {
    // ptable_data_t ptable_data = ptable_data_opt1; /* see tables_ptable_data.c for alternatives */
    
    if (verbose == 1){
        fprintf(stderr, "Using pruning table %s\n", ptable_data.name);
    }

    cube_tables_load_ptable(&ptable_data);
    if (!ptable_data.ptable_is_loaded){
        fprintf(stderr, "ptable error?\n");
        return false;
    }

    // not in use here, but is used by the solver.
    // hence, we need to make sure that it is loaded.
    void* _t = get_sym_table_e_index();
    if (!_t)
    {
        fprintf(stderr, "Could not load sym_table_e_index. Have you initialized it?\n");
        return false;
    }

    // we collect some stats along the way.
    struct stats* stats = malloc(sizeof(struct stats));
    init_stats(stats, number_of_solutions);

    // at the moment nissing does not play well with
    // finding multiple solutions.
    // for instance: R (U R) would be a solution of length 3...
    // even though it is the same as just U'
    if (number_of_solutions > 1)
    {
        enable_niss = false;
    }
    // actually search.
    IDA(cube, ptable_data, stats, number_of_solutions, verbose);

    if (verbose == 1)
    {
        print_stats(stats);
    }

    memcpy(solutions, stats->solutions, sizeof(int) * 20 * number_of_solutions);

    free(stats->solution);
    free(stats->solution_inv);
    free(stats->solutions);
    free(stats);

    return true;
}
