#include "solver_pipeline.h"


void solver_pipeline(cube_t c, int* solutions, struct arguments arguments, solving_step** steps){
    for (int i = 0; i < arguments.step_count; i++){
        solving_step* ss = steps[i];

        // not sure if this continues on the previous solve or if it uses the same scramble?
        if (cube_solvers_solve_cube(c, solutions, arguments.number_of_solutions,
                                    arguments.verbose, ss))
        {
            cube_print_solutions(solutions, arguments.number_of_solutions, arguments.verbose);

            // we do pipeline mode if user has supplied multiple steps
            if (arguments.step_count > 1){
                for (int i = 0; i < 20; i++){
                    if (solutions[i] != -1) {
                        cube_move_apply_move(&c, solutions[i]);
                    }
                    else {
                        break;
                    }
                }
            }
        }
        else
        {
            printf("Could not solve the cube :(\n");
        }
    }
}

typedef struct sol_and_length {
    int length;
    int* solution[20];
} sol_and_length;


void solver_beam_search(cube_t c, int* solutions, struct arguments arguments, solving_step** steps){
    int num_sols = arguments.number_of_solutions;
    cube_t cube_arr[num_sols];
    cube_arr[0] = c;
    int len_cube_arr = 1;

    for (int i = 0; i < arguments.step_count; i++){
        solving_step* ss = steps[i];
        
        int*  sols = malloc(sizeof(int) * 20 * arguments.number_of_solutions * len_cube_arr);

        // here we need to find num_sols solutions on each of the cubes in cube_arr
        for (int j = 0; j < len_cube_arr; j++){
            cube_t cube = cube_arr[j];
            int* sol_j = &sols[sizeof(int) * 20 * arguments.number_of_solutions * j];

            if (cube_solvers_solve_cube(cube, sol_j, arguments.number_of_solutions,
                                    arguments.verbose, ss))
            {
                cube_print_solutions(sol_j, num_sols, arguments.verbose);
            }
            else{
                printf("Could not solve step!\n");
            }
        }



        // sort and pick the num_sols shortest solutions for the next step.
        // todo: for now, just pick the num_sols first ones from the first cube
        // for now, just keep one cube
        cube_t cube = cube_arr[0];
        for (int m = 0; m < 20; m++){
            if (sols[m] == -1) {
                cube_move_apply_move(&cube, sols[m]);
            }
            else {
                break;
            }
        }
        cube_arr[0] = cube;
        free(sols);
    }

        // cube_t cube = cube_arr[0];
        // for (int j = 0; j < num_sols; j++){
        //     cube_t cube2 = cube;
        //     for (int m = 0; m < 20; m++){
        //         if (sols[m + sizeof(int) * 20 * j] == -1) {
        //             cube_move_apply_move(&cube2, sols[m + sizeof(int) * 20 * j]);
        //         }
        //         else {
        //             break;
        //         }
        //     }
        //     cube_arr[j] = cube2;
        // }
        // len_cube_arr = num_sols;
        // free(sols);
    // }
}