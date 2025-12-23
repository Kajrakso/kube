#include "solver.h"

void init_stats(struct solver_stats* stats, int max_num_sols) {
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

void print_stats(struct solver_stats* stats) {
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

void save_solution_to_solutions(struct solver_stats* s) {
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


static bool
TreeSearch(cube_t* cube, solving_step* ss, struct search_data s_data, struct solver_stats* stats) {
    stats->no_nodes_visited++;
    int  remaining_moves = s_data.remaining_moves;
    int  prev_move       = s_data.prev_move;
    int  prev_move_inv   = s_data.prev_move_inv;
    bool is_inv          = s_data.is_inv;
    int  max_num_sols    = s_data.max_num_sols;
    bool enable_niss     = s_data.enable_niss;


    if (remaining_moves == 0)
    {
        if (ss->cube_is_solved(cube))
        {
            save_solution_to_solutions(stats);
            stats->num_sol_found++;

            // keep searching if we
            // have not found enough solutions
            return stats->num_sol_found == max_num_sols;
        }
        else
        {
            return false;
        }
    }

    if (ss->p_data != NULL && ss->heuristic_func != NULL)
    {
        if (ss->heuristic_func(cube, ss->p_data) > remaining_moves)
        {
            stats->no_nodes_pruned++;
            return false;
        }
    }

    // we extract the move_mask for this node.
    uint32_t mm = move_mask[prev_move];

    for (int move = 0; move < NMOVES; move++)
    {
        // check if we actually need to do move.
        if (!(mm & (1 << move)))
            continue;

        cube_move_apply_move(cube, move);

        stats->solution[GODS_NO - remaining_moves] = move;

        struct search_data s_data_next = {.remaining_moves = remaining_moves - 1,
                                          .prev_move       = move,
                                          .prev_move_inv   = prev_move_inv,
                                          .is_inv          = is_inv,
                                          .max_num_sols    = max_num_sols,
                                          .enable_niss     = enable_niss};
        bool               found       = TreeSearch(cube, ss, s_data_next, stats);

        cube_move_apply_move(cube, get_inv_move(move));

        stats->solution[GODS_NO - remaining_moves] = -1;

        if (found)
        {
            return true;
        }
    }

    return false;
}

void IDA(cube_t cube, solving_step* ss, struct solver_stats* stats, int max_num_sols, int verbose) {

    bool stop_search = false;
    if (verbose == 1)
    {
        fprintf(stderr, "\tsolving_type: %i\n", ss->solving_type);

        if (ss->p_data == NULL)
        {
            fprintf(stderr, "\tsolving_step does not have associated pruning_data\n");
        }
        // else
        // {
        //     fprintf(stderr, "\tp_data ptr: %lu\n", (unsigned long) ss->p_data);
        //     fprintf(stderr, "\tp_data->ptable ptr: %lu\n", (unsigned long) ss->p_data->ptable);
        //     fprintf(stderr, "\tptable is %sloaded!\n", ss->p_data->ptable_is_loaded ? "" : "not ");
        // }

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

        struct search_data s_data = {.remaining_moves = depth,
                                     .prev_move       = 18,
                                     .prev_move_inv   = 18,
                                     .is_inv          = false,
                                     .max_num_sols    = max_num_sols,
                                     .enable_niss     = false};
        stop_search               = TreeSearch(&cube, ss, s_data, stats);

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

bool cube_solvers_solve_cube(
  cube_t cube, int* solutions, int number_of_solutions, int verbose, solving_step* ss) {
    // we collect some stats along the way.
    struct solver_stats* stats = malloc(sizeof(struct solver_stats));
    init_stats(stats, number_of_solutions);

    // at the moment nissing does not play well with
    // finding multiple solutions.
    // for instance: R (U R) would be a solution of length 3...
    // even though it is the same as just U'
    bool enable_niss = true;
    if (number_of_solutions > 1)
    {
        enable_niss = false;
    }

    // how to handle solving step with no p_data?
    if (ss->p_data != NULL)
    {
        if (verbose == 1)
        {
            fprintf(stderr, "\tUsing pruning table %s\n", ss->p_data->name);
        }

        cube_tables_load_ptable(ss->p_data);
        if (!ss->p_data->ptable_is_loaded)
        {
            fprintf(stderr, "\tptable error?\n");
            return false;
        }
    }


    // actually search
    if (ss->solving_type == SOLVE_FIN)
    {
        // not in use here, but is used by the solver.
        // hence, we need to make sure that it is loaded.
        void* _t = get_sym_table_e_index();
        if (!_t)
        {
            fprintf(stderr, "\tCould not load sym_table_e_index. Have you initialized it?\n");
            return false;
        }

        // using the solver with fancy nissing tricks
        IDA_fin(cube, ss->p_data, stats, number_of_solutions, verbose, enable_niss);
    }
    else
    {
        if (ss->solving_type == SOLVE_HTR || ss->solving_type == SOLVE_DR)
        {
            void* _t = get_dr_subsets();
            if (!_t)
            {
                fprintf(stderr, "\tCould not load dr_subsets. Have you initialized it?\n");
                return false;
            }
        }

        IDA(cube, ss, stats, number_of_solutions, verbose);
    }
    if (verbose == 1)
    {
        print_stats(stats);
    }

    memcpy(solutions, stats->solutions, sizeof(int) * GODS_NO * number_of_solutions);

    free(stats->solution);
    free(stats->solution_inv);
    free(stats->solutions);
    free(stats);

    return true;
}
