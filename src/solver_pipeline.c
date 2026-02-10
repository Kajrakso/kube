#include "solver_pipeline.h"


void solver_pipeline(cube_t c, struct arguments arguments, solving_step** steps){
    for (int i = 0; i < arguments.step_count; i++){
        solving_step* ss = steps[i];

        // prepare a solution set
        SolutionSet solution_set;
        solutionset_init(&solution_set, arguments.number_of_solutions);
        
        if (cube_solvers_solve_cube(c, &solution_set, arguments.number_of_solutions,
                                    arguments.verbose, ss))
        {
            cube_print_solution_set(&solution_set, arguments.verbose);

            // we do pipeline mode if user has supplied multiple steps
            if (arguments.step_count > 1){
                Solution solution = solution_set.data[0];
                for (int move_idx = 0; move_idx < solution.length; move_idx++){
                    int move = solution.moves[move_idx];
                    cube_move_apply_move(&c, move);
                }
            }
        }
        else
        {
            printf("Could not solve the cube :(\n");
        }

        solutionset_free(&solution_set);
    }
}

void solver_beam_search(cube_t c, struct arguments arguments, solving_step** steps){
    /* Idea: For each step in steps, pick the arguments.numbers_of_solutions shortest solutions so far
     * and find the arguments.number_of_solutions shortest solutions to the next step.
     * Then, continue like this until the end, where we pick the arguments.number_of_solutions shortest ones */
    int beam_width = arguments.number_of_solutions;

    /* Maybe return an array of length arguments.step_count of solution_sets,
     * where each solutionset has length beam_width. */


    SolutionSet current;
    solutionset_init(&current, beam_width);
    Solution empty_sol;
    solution_init(&empty_sol);
    solutionset_add_copy(&current, &empty_sol);

    for (int step_idx = 0; step_idx < arguments.step_count; step_idx++){
        solving_step* ss = steps[step_idx];

        // prepare a solution set
        SolutionSet next;
        solutionset_init(&next, beam_width*beam_width);

        for (int i = 0; i < current.count; i++){
            Solution* partial = &current.data[i];
            cube_t cube_copy = c;

            cube_move_apply_multiple_moves(&cube_copy, partial->moves, partial->length);

            // expand each of the current solutions by the beam width
            SolutionSet expanded;
            solutionset_init(&expanded, beam_width);

            cube_solvers_solve_cube(
                cube_copy,
                &expanded,
                arguments.number_of_solutions,
                arguments.verbose,
                ss
            );
            
            SolutionSet temp;
            solutionset_merge_with_prefix(&temp, &expanded, partial);
            solutionset_append(&next, &temp);

            solutionset_free(&temp);
            solutionset_free(&expanded);
        }

        // keep only the beam_width shortest solutions
        solutionset_trim_shortest(&next, beam_width);

        solutionset_free(&current);
        current = next;
    }

    cube_print_solution_set(&current, arguments.verbose);

}
