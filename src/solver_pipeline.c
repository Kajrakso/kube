#include "solver_pipeline.h"


void solver_pipeline(cube_t c, struct arguments arguments, solving_step** steps) {
    for (int i = 0; i < arguments.step_count; i++)
    {
        solving_step* ss = steps[i];

        // prepare a solution set
        SolutionSet solution_set;
        solutionset_init(&solution_set, arguments.number_of_solutions);

        if (cube_solvers_solve_cube(c, &solution_set, arguments.number_of_solutions,
                                    arguments.verbose, ss))
        {
            cube_print_solution_set(&solution_set, arguments.verbose);

            // we do pipeline mode if user has supplied multiple steps
            if (arguments.step_count > 1)
            {
                Solution solution = solution_set.data[0];
                for (int move_idx = 0; move_idx < solution.length; move_idx++)
                {
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


void solver_beam_search(cube_t c, struct arguments arguments, solving_step** steps) {
    /* Idea: For each step in steps, pick the arguments.numbers_of_solutions solutions with the lowest heuristic
     * so far and find the arguments.number_of_solutions shortest solutions to the next step.
     * Then, continue like this until the end, where we pick the arguments.number_of_solutions shortest ones */
    int beam_width = arguments.number_of_solutions;

    // keep track of the solutions so far.
    PipelineSolutionSet current;
    pipelinesolutionset_init(&current, beam_width);

    // add a pipeline solution with empty step just to get the solver started.
    // ÆSJ Må fikse
    PipelineSolution empty_pipeline_solution;
    pipelinesolution_init(&empty_pipeline_solution);
    PipelineSolutionStep empty_pipeline_solution_step;
    empty_pipeline_solution_step.starts_on_inverse = false;
    Solution empty;
    solution_init(&empty);
    empty_pipeline_solution_step.solution = &empty;
    pipelinesolution_add_copy(&empty_pipeline_solution, &empty_pipeline_solution_step);
    // pipelinesolutionset_add_copy(&current, &empty_pipeline_solution);

    PipelineSolution* pip_sol_i = &empty_pipeline_solution;
    for (int step_idx = 0; step_idx < arguments.step_count; step_idx++)
    {
        solving_step* ss = steps[step_idx];

        // prepare a new solution set
        PipelineSolutionSet next;
        pipelinesolutionset_init(&next, beam_width * beam_width
                                          * 2);  // mult. by 2 since we search normal + inverse

        int i = 0;
        do
        {
            // currently we have to start from the scramble each time.
            cube_t cube = c;

            // prepare a temp pipeline solution set
            PipelineSolutionSet expanded_pipeline;
            pipelinesolutionset_init(&expanded_pipeline, beam_width * 2);

            if (current.count > 0)
            {
                pip_sol_i = &current.data[i];
                apply_pipelinesolution(&cube, pip_sol_i);
            }

            // expand each of the current solutions by the beam width
            SolutionSet expanded;
            solutionset_init(&expanded, beam_width);

            cube_solvers_solve_cube(cube, &expanded, arguments.number_of_solutions,
                                    arguments.verbose, ss);
            append_copy_solutionset_to_pipelinesolutionset(&expanded_pipeline, &expanded, false);

            SolutionSet expanded_inv;
            solutionset_init(&expanded_inv, beam_width);

            cube_t cube_inv = cube_operation_inverse(cube);
            cube_solvers_solve_cube(cube_inv, &expanded_inv, arguments.number_of_solutions,
                                    arguments.verbose, ss);
            append_copy_solutionset_to_pipelinesolutionset(&expanded_pipeline, &expanded_inv, true);

            // we need to merge the solutions for the next step
            // with the solution we have found so far (but skip this
            // if we just started the search)
            if (current.count > 0)
            {
                PipelineSolutionSet temp;
                pipelinesolutionset_merge_with_prefix(&temp, &expanded_pipeline, pip_sol_i);
                pipelinesolutionset_append(&next, &temp);
                pipelinesolutionset_free(&temp);
            }
            else
            {
                pipelinesolutionset_append(&next, &expanded_pipeline);
            }

            pipelinesolutionset_free(&expanded_pipeline);
            solutionset_free(&expanded);
            solutionset_free(&expanded_inv);
        } while (++i < current.count);


        // we need the next solving step to calculate the heuristic function
        solving_step* next_ss = NULL;
        if (step_idx + 1 < arguments.step_count)
        {
            next_ss = steps[step_idx + 1];
        }

        // keep only the beam_width best solutions so far (as given by the score)
        pipelinesolutionset_compute_scores(c, &next, next_ss);
        pipelinesolutionset_trim_shortest(&next, beam_width);

        pipelinesolutionset_free(&current);
        current = next;
    }

    cube_print_pipelinesolution_set(&current, arguments.verbose);
    pipelinesolutionset_free(&current);
}
