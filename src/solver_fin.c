#include "solver.h"

// work in progress

static bool TreeSearch_fin(cube_t*       cube,
                       ptable_data_t* ptable_data,
                       struct search_data s_data,
                       struct solver_stats* stats
    ) {
    stats->no_nodes_visited++;
    int remaining_moves = s_data.remaining_moves;
    int prev_move = s_data.prev_move;
    int prev_move_inv = s_data.prev_move_inv;
    bool is_inv = s_data.is_inv;
    int max_num_sols = s_data.max_num_sols;
    bool enable_niss = s_data.enable_niss;


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
            stats->solution[GODS_NO - remaining_moves] = move;
        else
            stats->solution_inv[GODS_NO - remaining_moves] = move;

        struct search_data s_data_next = {
            .remaining_moves = remaining_moves - 1,
            .prev_move = !is_inv ? move : prev_move,
            .prev_move_inv = is_inv ? move : prev_move_inv,
            .is_inv = is_inv,
            .max_num_sols = max_num_sols,
            .enable_niss = enable_niss
        };
        bool found = TreeSearch_fin(cube, ptable_data, s_data_next, stats);

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

void IDA_fin(cube_t cube,
        ptable_data_t* ptable_data,
        struct solver_stats* stats, int max_num_sols, int verbose, bool niss) {

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
            .enable_niss = niss
        };
        stop_search = TreeSearch_fin(&cube, ptable_data, s_data, stats);

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
