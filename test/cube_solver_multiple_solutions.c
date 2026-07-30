#include "criterion/criterion.h"

#include "../src/cli.h"
#include "../src/solver.h"
#include "../src/utils.h"

Test(solver_multiple_solutions, solved_state){
    cube_t c = cube_create_new_cube();
    int number_of_solutions = 20;
    int max_depth = 20;
    int verbose = false;
    int number_of_threads = 16;

    SolutionSet ss;
    solutionset_init(&ss, (size_t)number_of_solutions);

    cube_solvers_solve_cube(c, &ss, number_of_solutions, max_depth, verbose, number_of_threads, &fin);

    cr_assert_eq(ss.count, number_of_solutions); 
    
    int solution_lengths[20];
    for (int i = 0; i < 20; i++){
        solution_lengths[i] = 0;
    }

    for (size_t i = 0; i < ss.count; i++) {
        solution_lengths[ss.data[i].length]++;
    }
    cr_assert_eq(solution_lengths[0], 1);
    cr_assert_eq(solution_lengths[8], 18);
    cr_assert_eq(solution_lengths[9], 1);
    cr_assert_eq(solution_lengths[10], 0);

    solutionset_free(&ss);
}


Test(solver_multiple_solutions, 9_move_optimal){
    cube_t c = cube_create_new_cube();
    cube_move_apply_move_string(&c, "R U F L B2 D' F' U L2");

    int number_of_solutions = 20;
    int max_depth = 20;
    int verbose = false;
    int number_of_threads = 16;

    SolutionSet ss;
    solutionset_init(&ss, (size_t)number_of_solutions);

    cube_solvers_solve_cube(c, &ss, number_of_solutions, max_depth, verbose, number_of_threads, &fin);

    cr_assert_eq(ss.count, number_of_solutions); 
    
    int solution_lengths[20];
    for (int i = 0; i < 20; i++){
        solution_lengths[i] = 0;
    }

    for (size_t i = 0; i < ss.count; i++) {
        solution_lengths[ss.data[i].length]++;
    }

    // for (int i = 0; i < 20; i++){
    //     printf("%i: %i\n",i, solution_lengths[i]);
    // }
    // cube_print_solution_set(&ss, 1);
    cr_assert_eq(solution_lengths[9], 1);
    cr_assert_eq(solution_lengths[14], 15);
    cr_assert_eq(solution_lengths[15], 4);
    cr_assert_eq(solution_lengths[16], 0);
    cr_assert_eq(solution_lengths[17], 0);

    solutionset_free(&ss);
}


Test(solver_multiple_solutions, 4_move_optimal_15_thread){
    cube_t c = cube_create_new_cube();
    cube_move_apply_move_string(&c, "R2 U L2 F");

    int number_of_solutions = 20;
    int max_depth = 20;
    int verbose = false;
    int number_of_threads = 15;

    SolutionSet ss;
    solutionset_init(&ss, (size_t)number_of_solutions);

    cube_solvers_solve_cube(c, &ss, number_of_solutions, max_depth, verbose, number_of_threads, &fin);

    cr_assert_eq(ss.count, number_of_solutions); 
    
    int solution_lengths[20];
    for (int i = 0; i < 20; i++){
        solution_lengths[i] = 0;
    }

    for (size_t i = 0; i < ss.count; i++) {
        solution_lengths[ss.data[i].length]++;
    }
    
    cr_assert_eq(solution_lengths[3], 0);
    cr_assert_eq(solution_lengths[4], 1);
    cr_assert_eq(solution_lengths[5], 0);
    cr_assert_eq(solution_lengths[6], 0);
    cr_assert_eq(solution_lengths[7], 0);
    cr_assert_eq(solution_lengths[8], 1);
    cr_assert_eq(solution_lengths[9], 5);
    cr_assert_eq(solution_lengths[10], 13);
    cr_assert_eq(solution_lengths[11], 0);
    cr_assert_eq(solution_lengths[12], 0);

    solutionset_free(&ss);
}

Test(solver_multiple_solutions, 4_move_optimal_1_thread){
    cube_t c = cube_create_new_cube();
    cube_move_apply_move_string(&c, "R2 U L2 F");

    int number_of_solutions = 20;
    int max_depth = 20;
    int verbose = false;
    int number_of_threads = 1;

    SolutionSet ss;
    solutionset_init(&ss, (size_t)number_of_solutions);

    cube_solvers_solve_cube(c, &ss, number_of_solutions, max_depth, verbose, number_of_threads, &fin);

    cr_assert_eq(ss.count, number_of_solutions); 
    
    int solution_lengths[20];
    for (int i = 0; i < 20; i++){
        solution_lengths[i] = 0;
    }

    for (size_t i = 0; i < ss.count; i++) {
        solution_lengths[ss.data[i].length]++;
    }
    
    cr_assert_eq(solution_lengths[3], 0);
    cr_assert_eq(solution_lengths[4], 1);
    cr_assert_eq(solution_lengths[5], 0);
    cr_assert_eq(solution_lengths[6], 0);
    cr_assert_eq(solution_lengths[7], 0);
    cr_assert_eq(solution_lengths[8], 1);
    cr_assert_eq(solution_lengths[9], 5);
    cr_assert_eq(solution_lengths[10], 13);
    cr_assert_eq(solution_lengths[11], 0);
    cr_assert_eq(solution_lengths[12], 0);

    solutionset_free(&ss);
}

Test(solver_multiple_solutions, 17_move_optimal_16_thread){
    cube_t c = cube_create_new_cube();
    cube_move_apply_move_string(&c, "U' R2 B2 D2 R B' D2 F D' L2 F2 L2 D2 L2 B2 U2 F' B2 R2 B'");

    int number_of_solutions = 20;
    int max_depth = 20;
    int verbose = false;
    int number_of_threads = 16;

    SolutionSet ss;
    solutionset_init(&ss, (size_t)number_of_solutions);

    cube_solvers_solve_cube(c, &ss, number_of_solutions, max_depth, verbose, number_of_threads, &fin);

    cr_assert_eq(ss.count, number_of_solutions); 
    
    int solution_lengths[20];
    for (int i = 0; i < 20; i++){
        solution_lengths[i] = 0;
    }

    for (size_t i = 0; i < ss.count; i++) {
        solution_lengths[ss.data[i].length]++;
    }
    
    cr_assert_eq(solution_lengths[3], 0);
    cr_assert_eq(solution_lengths[4], 0);
    cr_assert_eq(solution_lengths[5], 0);
    cr_assert_eq(solution_lengths[6], 0);
    cr_assert_eq(solution_lengths[7], 0);
    cr_assert_eq(solution_lengths[8], 0);
    cr_assert_eq(solution_lengths[9], 0);
    cr_assert_eq(solution_lengths[10], 0);
    cr_assert_eq(solution_lengths[11], 0);
    cr_assert_eq(solution_lengths[12], 0);
    cr_assert_eq(solution_lengths[16], 0);
    cr_assert_eq(solution_lengths[17], 1);
    cr_assert_eq(solution_lengths[18], 7);
    cr_assert_eq(solution_lengths[19], 12);

    solutionset_free(&ss);
}
