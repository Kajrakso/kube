#include "criterion/criterion.h"

#include "../src/cli.h"
#include "../src/solver.h"
#include "../src/utils.h"

Test(solver_fin, solved_is_solved){
    cube_t c = cube_create_new_cube();
    int number_of_solutions = 1;
    int max_depth = 20;
    int verbose = false;
    int number_of_threads = 16;

    SolutionSet ss;
    solutionset_init(&ss, (size_t)number_of_solutions);

    cube_solvers_solve_cube(c, &ss, number_of_solutions, max_depth, verbose, number_of_threads, &fin);
    cr_assert_eq(ss.count, number_of_solutions); 
    cr_assert_eq(ss.data[0].length, 0);

    solutionset_free(&ss);
}


Test(solver_fin, solver_finds_number_of_solutions){
    cube_t c = cube_create_new_cube();
    int number_of_solutions = rand() % 20 + 1;
    int max_depth = 20;
    int verbose = false;
    int number_of_threads = 16;

    SolutionSet ss;
    solutionset_init(&ss, (size_t)number_of_solutions);

    cube_solvers_solve_cube(c, &ss, number_of_solutions, max_depth, verbose, number_of_threads, &fin);
    cr_assert_eq(ss.count, number_of_solutions); 

    solutionset_free(&ss);
}

/* -------------------------------------- */
/* 1 movers */
/* -------------------------------------- */

#define TEST_number_of_scrambles_solve_and_test_1_movers 18
void solve_and_test_1_movers(int nthreads){
    // prepare scrambles
    cube_t cubes[TEST_number_of_scrambles_solve_and_test_1_movers] = {};
    for (int i = 0; i < TEST_number_of_scrambles_solve_and_test_1_movers; i++){ cubes[i] = cube_create_new_cube(); }
    cube_move_apply_move_string(&cubes[0], "R3");
    cube_move_apply_move_string(&cubes[1], "R2");
    cube_move_apply_move_string(&cubes[2], "R1");
    cube_move_apply_move_string(&cubes[3], "L3");
    cube_move_apply_move_string(&cubes[4], "L2");
    cube_move_apply_move_string(&cubes[5], "L1");
    cube_move_apply_move_string(&cubes[6], "F3");
    cube_move_apply_move_string(&cubes[7], "F2");
    cube_move_apply_move_string(&cubes[8], "F1");
    cube_move_apply_move_string(&cubes[9], "B3");
    cube_move_apply_move_string(&cubes[10], "B2");
    cube_move_apply_move_string(&cubes[11], "B1");
    cube_move_apply_move_string(&cubes[12], "U3");
    cube_move_apply_move_string(&cubes[13], "U2");
    cube_move_apply_move_string(&cubes[14], "U1");
    cube_move_apply_move_string(&cubes[15], "D3");
    cube_move_apply_move_string(&cubes[16], "D2");
    cube_move_apply_move_string(&cubes[17], "D1");

    // predefined unique optimal solutions
    int solutions[TEST_number_of_scrambles_solve_and_test_1_movers][1] = {
        {R1},
        {R2},
        {R3},
        {L1},
        {L2},
        {L3},
        {F1},
        {F2},
        {F3},
        {B1},
        {B2},
        {B3},
        {U1},
        {U2},
        {U3},
        {D1},
        {D2},
        {D3},
    };

    int number_of_solutions = 1;
    int max_depth = 20;
    int verbose = false;
    int number_of_threads = nthreads;
    
    for (int i = 0; i < TEST_number_of_scrambles_solve_and_test_1_movers; i++){
        SolutionSet ss;
        solutionset_init(&ss, (size_t)number_of_solutions);

        cube_solvers_solve_cube(cubes[i], &ss, number_of_solutions, max_depth, verbose, number_of_threads, &fin);

        cr_assert(arr_eq_int(solutions[i], ss.data->moves, 1));
       
        solutionset_free(&ss);
    }
}


Test(solver_fin, solver_1move_scrambles_correctly_1thread){
    solve_and_test_1_movers(1);
}


Test(solver_fin, solver_1move_scrambles_correctly_16thread){
    solve_and_test_1_movers(16);
}


/* -------------------------------------- */
/* 4 movers */
/* -------------------------------------- */

#define TEST_number_of_scrambles_solve_and_test_4_movers 18
void solve_and_test_4_movers(int nthreads){
    // prepare scrambles
    cube_t cubes[TEST_number_of_scrambles_solve_and_test_4_movers] = {};
    for (int i = 0; i < TEST_number_of_scrambles_solve_and_test_4_movers; i++){ cubes[i] = cube_create_new_cube(); }
    cube_move_apply_move_string(&cubes[0], "R3 U1 L3 D1");
    cube_move_apply_move_string(&cubes[1], "R2 U2 R2 D2");
    cube_move_apply_move_string(&cubes[2], "R1 F2 L2 D3");
    cube_move_apply_move_string(&cubes[3], "L3 U2 F3 U2");
    cube_move_apply_move_string(&cubes[4], "L2 B2 L3 B3");
    cube_move_apply_move_string(&cubes[5], "L1 F3 U2 R2");
    cube_move_apply_move_string(&cubes[6], "F3 R2 U1 F2");
    cube_move_apply_move_string(&cubes[7], "F2 D2 R3 U2");
    cube_move_apply_move_string(&cubes[8], "F1 L1 B2 U1");
    cube_move_apply_move_string(&cubes[9], "B3 D3 R3 F3");
    cube_move_apply_move_string(&cubes[10], "B2 R1 U2 B3");
    cube_move_apply_move_string(&cubes[11], "B1 L2 U2 B1");
    cube_move_apply_move_string(&cubes[12], "U3 F1 U1 F1");
    cube_move_apply_move_string(&cubes[13], "U2 R2 U2 F1");
    cube_move_apply_move_string(&cubes[14], "U1 L3 D2 B1");
    cube_move_apply_move_string(&cubes[15], "D3 F1 R1 B1");
    cube_move_apply_move_string(&cubes[16], "D2 B2 U1 F1");
    cube_move_apply_move_string(&cubes[17], "D1 R1 F2 L1");

    // predefined unique optimal solutions
    int solutions[TEST_number_of_scrambles_solve_and_test_4_movers][4] = {
        {D3, L1, U3, R1},
        {D2, R2, U2, R2},
        {D1, L2, F2, R3},
        {U2, F1, U2, L1},
        {B1, L1, B2, L2},
        {R2, U2, F1, L3},
        {F2, U3, R2, F1},
        {U2, R1, D2, F2},
        {U3, B2, L3, F3},
        {F1, R1, D1, B1},
        {B1, U2, R3, B2},
        {B3, U2, L2, B3},
        {F3, U3, F3, U1},
        {F3, U2, R2, U2},
        {B3, D2, L1, U3},
        {B3, R3, F3, D1},
        {F3, U3, B2, D2},
        {L3, F2, R3, D3},
    };

    int number_of_solutions = 1;
    int max_depth = 20;
    int verbose = false;
    int number_of_threads = nthreads;
    
    for (int i = 0; i < TEST_number_of_scrambles_solve_and_test_4_movers; i++){
        SolutionSet ss;
        solutionset_init(&ss, (size_t)number_of_solutions);

        cube_solvers_solve_cube(cubes[i], &ss, number_of_solutions, max_depth, verbose, number_of_threads, &fin);

        cr_assert(arr_eq_int(solutions[i], ss.data->moves, 4));
       
        solutionset_free(&ss);
    }
}


Test(solver_fin, solver_4move_scrambles_correctly_1thread){
    solve_and_test_4_movers(1);
}


Test(solver_fin, solver_4move_scrambles_correctly_16thread){
    solve_and_test_4_movers(16);
}



/* test optimal length of a given set of scrambles is correct */

#define TEST_number_of_scrambles_solve_and_test_15_movers 433
void solve_and_test_15_movers(int nthreads){
    // prepare scrambles
    cube_t cubes[TEST_number_of_scrambles_solve_and_test_15_movers] = {};
    for (int i = 0; i < TEST_number_of_scrambles_solve_and_test_15_movers; i++){ cubes[i] = cube_create_new_cube(); }
    cube_move_apply_move_string(&cubes[0], "U1 D2 F2 D2 F3 L2 U3 L3 U2 F1 D2 B1 D2 B3 R2 F3 L2 U2 B1 D2 ");
    cube_move_apply_move_string(&cubes[1], "D3 R1 U2 L2 U1 R2 F2 L2 D3 R2 D2 B2 U1 B1 F3 D3 B3 R1 D3 L2 ");
    cube_move_apply_move_string(&cubes[2], "F2 R1 B2 D2 R2 D2 U1 L2 F2 D2 F2 U3 L2 F2 B3 R3 D2 B2 R1 D3 L1 ");
    cube_move_apply_move_string(&cubes[3], "U3 B2 U3 L1 F3 U1 L3 F3 L3 U2 D2 B2 R2 D2 B1 L2 D2 B1 D2 R2 B1 ");
    cube_move_apply_move_string(&cubes[4], "R2 U1 L2 D2 U1 B2 R2 U3 L2 R2 D2 L1 F2 U2 B3 L2 F1 U2 R2 U1 ");
    cube_move_apply_move_string(&cubes[5], "B1 R2 U1 D2 F3 U2 R1 L3 B2 R2 D3 R2 D3 B2 D1 ");
    cube_move_apply_move_string(&cubes[6], "B2 R2 B3 F2 D3 R2 B2 L2 F2 D1 B2 U1 R2 L3 F1 U3 R1 D2 B2 D1 ");
    cube_move_apply_move_string(&cubes[7], "R2 B3 F3 R2 B1 D2 F1 L2 R2 D2 R1 F2 U3 B3 R1 U1 R2 B3 ");
    cube_move_apply_move_string(&cubes[8], "F3 L2 D3 B1 R1 L3 F1 U2 F2 L2 F2 U2 L2 U1 L2 B2 U1 R3 L2 ");
    cube_move_apply_move_string(&cubes[9], "L2 U2 B3 D2 F3 D2 L2 D2 B3 L2 D1 B1 F3 L3 D1 F1 U1 F1 D3 ");
    cube_move_apply_move_string(&cubes[10], "F1 B1 L1 D1 F1 B2 U3 F1 U1 F2 L1 F2 R3 F2 L2 U2 ");
    cube_move_apply_move_string(&cubes[11], "B2 U1 F2 D1 L2 B2 L2 B2 D2 U3 L1 U1 F3 R2 D1 F1 R1 F3 D2 ");
    cube_move_apply_move_string(&cubes[12], "B1 R3 F3 L3 F1 B3 R1 L2 D3 F2 D2 R2 F2 R2 D3 L2 F2 D1 F1 ");
    cube_move_apply_move_string(&cubes[13], "B2 L2 F2 L2 U3 B3 U3 D1 B2 D1 F2 R2 U3 B2 D2 F2 ");
    cube_move_apply_move_string(&cubes[14], "F3 D2 U2 F1 D2 B1 D2 B3 U2 D1 L2 D2 L3 B2 L2 D2 U2 ");
    cube_move_apply_move_string(&cubes[15], "R2 D1 U2 L2 U3 B2 F2 L3 D1 F3 U2 F2 L3 R3 F1 ");
    cube_move_apply_move_string(&cubes[16], "B2 F2 U2 L3 D2 U2 F2 L3 U2 L1 D1 B2 U3 F1 L3 D2 F3 U1 L1 ");
    cube_move_apply_move_string(&cubes[17], "F2 L3 B1 U1 D3 R1 F3 B3 R2 U1 B2 D2 F2 D3 R2 D3 L2 U1 ");
    cube_move_apply_move_string(&cubes[18], "D2 F3 R2 F1 U2 F3 R2 B2 U2 L2 F3 R1 B1 F1 L1 R3 U2 F3 D3 B2 ");
    cube_move_apply_move_string(&cubes[19], "L2 D1 B2 D3 U1 F2 D3 L2 F2 U1 F2 B1 D3 B2 D3 L3 F3 U1 F1 L1 R3 ");
    cube_move_apply_move_string(&cubes[20], "U1 R2 B2 L2 U3 R2 F3 L1 D3 R3 D3 B2 U2 R3 D2 ");
    cube_move_apply_move_string(&cubes[21], "U1 B1 L3 B2 R1 D2 L1 B1 F2 U2 B2 U2 R1 F2 D2 L2 F2 L2 ");
    cube_move_apply_move_string(&cubes[22], "F1 D1 F3 R3 U1 B2 R3 B1 U1 D2 F2 U3 L2 D1 R2 L2 ");
    cube_move_apply_move_string(&cubes[23], "L3 D3 F2 D2 L2 F2 R2 F2 D3 R2 U1 R2 F2 B3 U1 B1 F2 L1 U3 L1 B3 ");
    cube_move_apply_move_string(&cubes[24], "F2 B1 D1 F2 B1 R2 L1 D3 L2 U3 F2 D1 R2 F2 U2 F2 U2 R1 U2 ");
    cube_move_apply_move_string(&cubes[25], "D2 R1 D3 L3 F2 U1 F3 R3 F3 R2 B1 R2 B3 R2 D2 F3 D2 ");
    cube_move_apply_move_string(&cubes[26], "U1 B3 D2 R2 D3 L2 B2 D1 F2 L2 R2 D1 U3 R1 B3 F2 D3 U2 F1 L3 ");
    cube_move_apply_move_string(&cubes[27], "U2 L3 B2 D1 U1 F2 D1 R2 D1 B2 F2 U1 L2 R2 B3 R2 U3 L1 F3 D1 U1 ");
    cube_move_apply_move_string(&cubes[28], "U1 D2 F2 R3 U2 B2 L2 D2 R1 D2 R2 F2 B1 R1 B3 U3 F2 U1 F3 D3 L3 ");
    cube_move_apply_move_string(&cubes[29], "L2 D3 F2 U1 B2 U2 B2 F3 L3 B3 R3 B3 L3 D3 R3 ");
    cube_move_apply_move_string(&cubes[30], "U1 R3 U2 D1 L3 U3 L3 U2 B3 R2 U2 F1 R2 L2 F3 U2 D2 R3 ");
    cube_move_apply_move_string(&cubes[31], "L2 F3 L2 R2 F1 U2 R2 U2 B1 U2 F3 D3 L1 U2 L1 R1 U3 F2 U1 F1 ");
    cube_move_apply_move_string(&cubes[32], "D3 R2 D1 L2 U3 B1 R3 B3 L3 D3 R2 U2 R1 B3 R3 ");
    cube_move_apply_move_string(&cubes[33], "D3 R1 B2 D3 F2 B3 U2 R2 D1 F2 B2 R2 U2 F2 B2 R3 D2 L1 D2 L3 ");
    cube_move_apply_move_string(&cubes[34], "U3 F3 R3 F2 B3 L3 U2 D3 R1 U2 R2 L2 B3 D2 F3 L2 B1 U2 F2 R2 ");
    cube_move_apply_move_string(&cubes[35], "F3 D3 L2 R2 D2 R2 U3 F2 D3 L2 D3 F3 R1 F3 D1 F1 L3 R3 F3 ");
    cube_move_apply_move_string(&cubes[36], "F2 R2 B3 U2 B3 R2 D2 B3 R2 B2 U2 F1 U3 B1 R1 F2 D1 U2 R2 D2 F3 ");
    cube_move_apply_move_string(&cubes[37], "D1 L2 R2 F3 L2 R2 U2 F1 R2 U2 F3 L2 B2 R3 D1 L3 F2 R1 D3 R2 ");
    cube_move_apply_move_string(&cubes[38], "U3 R2 D1 L1 B1 U3 F1 L1 F3 D2 R1 L2 B2 U2 F2 U2 R3 D2 B2 U2 R3 ");
    cube_move_apply_move_string(&cubes[39], "U1 R2 B2 R2 D1 U2 R2 D2 L2 F2 L2 U1 F3 L1 B2 U2 R1 F1 L2 F2 R3 ");
    cube_move_apply_move_string(&cubes[40], "B3 U3 F2 L2 F2 L3 U1 L2 D2 B3 D2 B2 D2 F1 R2 F3 R2 U2 R1 ");
    cube_move_apply_move_string(&cubes[41], "L3 U1 R1 D2 F2 U3 B1 U3 L2 B2 U2 R3 L3 B2 R1 ");
    cube_move_apply_move_string(&cubes[42], "U1 F2 D1 L1 B1 R2 B1 U1 F3 R3 D2 F2 L1 F2 L3 B2 U2 B2 L1 ");
    cube_move_apply_move_string(&cubes[43], "R3 F2 L2 U1 B2 U1 F2 U3 R2 U2 R2 F2 B1 R1 F3 R1 B2 L3 D3 L3 R2 ");
    cube_move_apply_move_string(&cubes[44], "U1 F3 R2 U2 L3 F1 R1 F3 B2 L2 B2 D3 R2 D3 L2 U2 F2 D3 L1 ");
    cube_move_apply_move_string(&cubes[45], "D2 U2 R1 F2 D2 U2 R2 B2 R1 D2 F2 B3 R3 D1 L3 U3 L1 R2 U3 B3 ");
    cube_move_apply_move_string(&cubes[46], "B1 U2 F3 R3 B2 D2 L1 F1 U3 L3 U2 L1 B2 U2 L1 D2 F2 L1 B2 R1 F2 ");
    cube_move_apply_move_string(&cubes[47], "L3 R2 F1 U2 B1 D2 B2 U2 R2 F1 R2 F3 D3 L2 B1 U3 L3 D1 L1 B2 L3 ");
    cube_move_apply_move_string(&cubes[48], "L2 U1 L3 F2 D1 F2 L2 U1 F3 D3 R3 B3 R1 F2 U3 ");
    cube_move_apply_move_string(&cubes[49], "D1 R1 L2 D2 B2 U1 L2 F3 D1 F2 L2 F3 D2 R2 F1 U2 R2 F1 D2 ");
    cube_move_apply_move_string(&cubes[50], "U3 L2 U1 R2 D1 B2 D3 R2 U1 B2 L2 R3 U1 B2 R3 F1 D3 R2 U2 F1 ");
    cube_move_apply_move_string(&cubes[51], "F2 U2 F2 R3 U2 F2 L3 R2 D2 F2 L1 U1 R1 U3 L1 R3 U1 R2 ");
    cube_move_apply_move_string(&cubes[52], "R1 B2 L1 D2 B2 U2 R3 B2 U2 F3 D3 U1 L1 D1 R3 ");
    cube_move_apply_move_string(&cubes[53], "L3 U3 B1 U1 D1 F3 B3 U3 R1 B2 D2 L3 D2 R1 U2 F2 D2 R3 D2 R1 U3 ");
    cube_move_apply_move_string(&cubes[54], "R3 U1 F2 D2 F1 R1 B1 U1 F2 R2 D2 F3 R2 D2 F3 R2 B1 R2 D2 R1 ");
    cube_move_apply_move_string(&cubes[55], "U2 F3 R3 D2 B2 L1 D3 B3 D2 F1 U2 L2 D2 F3 L2 B3 L2 ");
    cube_move_apply_move_string(&cubes[56], "U3 R1 B2 D1 L2 D2 R2 F2 L2 U1 B2 D2 R1 D2 B1 D2 U1 L2 B1 D1 ");
    cube_move_apply_move_string(&cubes[57], "B1 L3 F2 R2 B2 U1 R2 D3 R2 B2 U3 F3 L1 B2 L1 D2 F1 L1 B3 ");
    cube_move_apply_move_string(&cubes[58], "L1 B2 L2 D3 L3 D3 F1 B1 U2 L1 D2 L1 U2 B2 R2 D2 L1 B2 D2 B2 ");
    cube_move_apply_move_string(&cubes[59], "R2 D1 L2 F2 R2 U3 R2 D3 B2 U2 F2 R2 B3 U1 L3 F3 D3 B1 F2 L1 B1 ");
    cube_move_apply_move_string(&cubes[60], "U2 D1 B3 D3 F1 L1 F1 U1 R2 F2 D2 F2 R3 D2 F2 L1 D2 R1 F2 U1 ");
    cube_move_apply_move_string(&cubes[61], "R3 F2 R2 U1 R2 B2 D3 R2 D1 B2 U3 F2 D2 B3 U2 F3 D3 L1 D1 B1 ");
    cube_move_apply_move_string(&cubes[62], "L2 U1 F3 L2 B3 D3 L3 F2 L2 U2 F1 L2 D2 B2 U2 B2 L2 D2 L3 D3 ");
    cube_move_apply_move_string(&cubes[63], "B2 U2 R3 B1 D1 F1 D1 R2 B2 U2 D2 R2 F2 R1 U2 L3 F2 L2 B2 D3 B2 ");
    cube_move_apply_move_string(&cubes[64], "D3 L2 D1 L2 B2 D1 R2 U3 L2 D1 L2 U2 L1 U3 B2 D3 R3 D3 B3 D3 ");
    cube_move_apply_move_string(&cubes[65], "R3 F1 L2 D2 L2 D1 B2 F2 L2 U3 F2 L2 U1 L3 D1 U3 F1 U1 B3 R2 B1 ");
    cube_move_apply_move_string(&cubes[66], "L1 B2 L2 D1 B2 L2 D2 L2 F2 L2 U3 R2 D1 L1 B2 F2 D1 U2 F3 L2 U2 ");
    cube_move_apply_move_string(&cubes[67], "U3 F1 U1 F3 R3 D2 F1 D2 F2 R1 U2 L2 F2 R3 U2 R1 D2 L3 B2 D1 ");
    cube_move_apply_move_string(&cubes[68], "L2 U3 L2 D1 F3 L1 U2 F2 D3 R2 U2 L3 F2 D2 R1 F2 L2 B2 U2 B2 ");
    cube_move_apply_move_string(&cubes[69], "R2 U3 L2 F2 U3 B2 L2 D2 L2 F2 R3 F3 U1 F2 U1 B2 R2 B3 R1 ");
    cube_move_apply_move_string(&cubes[70], "R3 U1 L2 U2 L2 B2 D2 R3 U1 R2 F2 B1 R2 L2 F3 U2 F1 R2 F2 ");
    cube_move_apply_move_string(&cubes[71], "B3 R3 B2 D1 F2 D3 L2 U2 L2 U3 B2 F2 L3 U2 L2 U2 B3 D1 U3 ");
    cube_move_apply_move_string(&cubes[72], "F1 L3 B3 U3 L1 F3 L3 F1 B1 L2 F2 L2 F2 U3 D2 R2 L2 ");
    cube_move_apply_move_string(&cubes[73], "U3 F2 R2 F3 U2 F2 D2 F1 L2 R2 U2 F2 L1 U3 L3 B2 D1 F3 R1 B2 ");
    cube_move_apply_move_string(&cubes[74], "D3 B1 L2 B2 L2 F2 U2 R2 U3 B2 R2 B2 D3 R3 D3 L2 B1 U1 L2 B2 ");
    cube_move_apply_move_string(&cubes[75], "L2 D2 R2 B2 L2 D3 L2 U3 L2 R2 D2 B3 D1 R2 B3 L2 U2 F2 L3 F1 ");
    cube_move_apply_move_string(&cubes[76], "L1 B3 D2 F1 D2 R3 B1 D1 R3 U2 L3 F2 U2 R1 L1 F2 L1 B3 ");
    cube_move_apply_move_string(&cubes[77], "B3 D2 U2 L3 F2 L3 F2 L2 D2 L1 B2 U2 R2 U1 F2 L1 R1 U1 B1 F3 ");
    cube_move_apply_move_string(&cubes[78], "F1 U3 R1 U3 B3 U3 L2 D3 R1 F1 U2 L2 U2 R2 F2 U2 D2 B1 U2 F1 ");
    cube_move_apply_move_string(&cubes[79], "L2 B1 U2 B2 L2 U2 L2 B1 R2 F1 U2 D1 B3 R3 U1 F1 D3 R1 U3 R2 ");
    cube_move_apply_move_string(&cubes[80], "D1 R2 F2 U1 L2 U2 R2 D1 F2 R2 F2 U1 L3 B1 R1 F1 L3 R3 U1 L3 U3 ");
    cube_move_apply_move_string(&cubes[81], "F2 R3 U3 F2 L2 B2 L2 D3 U3 R2 U3 B2 U3 R2 F3 D3 R3 U2 B3 D1 ");
    cube_move_apply_move_string(&cubes[82], "L3 U3 R1 L1 D2 R1 U1 F1 L3 D2 B2 U3 D3 R2 D3 ");
    cube_move_apply_move_string(&cubes[83], "R2 B1 R2 U2 F1 L3 D1 B3 D1 B2 D2 F2 B2 L1 D2 L2 B2 U2 L1 D2 ");
    cube_move_apply_move_string(&cubes[84], "R2 U3 B1 R1 D2 B2 L1 U1 D2 L1 U2 L1 B2 R1 F2 R3 U2 B2 L1 D2 ");
    cube_move_apply_move_string(&cubes[85], "R2 U3 R2 U3 R2 B2 R2 U3 L2 D1 L2 B1 R1 D3 U1 L1 U2 F1 D3 R1 ");
    cube_move_apply_move_string(&cubes[86], "D1 L3 F3 U1 L3 D1 L1 B2 R2 L2 U3 D2 F2 D1 F2 L2 D2 F2 B2 ");
    cube_move_apply_move_string(&cubes[87], "U1 L2 U3 L2 F2 U3 L2 R2 U3 F2 L2 R2 F1 D3 B1 D3 F1 U1 R1 F1 R3 ");
    cube_move_apply_move_string(&cubes[88], "R2 D2 L3 D1 L2 B1 D1 B1 U2 R1 U2 B2 D2 R1 B2 ");
    cube_move_apply_move_string(&cubes[89], "B1 F1 L2 D2 U2 B1 L2 R2 D2 F3 R2 B1 U1 B3 F1 D3 U3 B3 D2 R3 U3 ");
    cube_move_apply_move_string(&cubes[90], "U1 R1 L3 D1 B1 L3 D2 B3 D3 U2 B1 L2 D2 L2 F2 U2 B1 U2 L2 F2 ");
    cube_move_apply_move_string(&cubes[91], "U1 F3 D2 F3 U2 D3 L3 F2 R2 D2 F3 R2 F3 R2 F3 R2 U2 B3 L2 U3 ");
    cube_move_apply_move_string(&cubes[92], "L2 U3 D1 R3 F1 D3 L2 F2 R1 D3 L2 F2 U3 D2 B2 U3 D2 L2 B2 ");
    cube_move_apply_move_string(&cubes[93], "R1 L1 F3 R3 D1 F1 L3 U2 B3 L2 F2 R2 U2 B3 R2 D2 B3 D2 B2 ");
    cube_move_apply_move_string(&cubes[94], "F2 R3 U1 L2 U1 L2 B2 D1 R2 F2 R2 D2 R2 U3 F1 R3 U2 R3 F2 D1 F3 ");
    cube_move_apply_move_string(&cubes[95], "B2 R1 B2 D2 F2 R1 F3 L3 B3 F3 L1 F3 D3 R3 B1 ");
    cube_move_apply_move_string(&cubes[96], "U3 B2 L3 U2 D1 R2 F3 R1 F1 U1 F2 D3 L2 U1 B2 U2 F2 U1 F2 D3 ");
    cube_move_apply_move_string(&cubes[97], "R3 F3 D3 F2 L3 B2 U2 L1 F1 D2 F2 R2 B3 R2 B1 U2 B3 L2 B3 ");
    cube_move_apply_move_string(&cubes[98], "B2 D2 U2 B3 D2 F3 R2 F3 R2 D2 U2 F2 R1 D2 R2 F1 R1 U2 ");
    cube_move_apply_move_string(&cubes[99], "R2 D3 F3 D2 R2 B3 L2 R2 B3 L2 F3 R2 B2 D3 U2 L2 F3 R1 B2 U1 ");
    cube_move_apply_move_string(&cubes[100], "R2 D1 F3 D2 F1 L1 F2 U1 F3 R2 U1 L2 U3 L2 D1 F2 L2 U3 F2 ");
    cube_move_apply_move_string(&cubes[101], "U1 B2 U2 F2 L2 U3 B2 U3 R2 U3 B2 L2 F1 U1 B2 L3 B1 L3 R2 B2 F2 ");
    cube_move_apply_move_string(&cubes[102], "F1 D2 L2 R2 B3 L2 B3 F3 R2 U2 R2 L3 D2 R2 U3 B1 U1 F2 D1 R2 ");
    cube_move_apply_move_string(&cubes[103], "F2 L3 U2 F3 U2 R2 F1 L3 F2 B2 L3 B2 L3 U2 R3 F2 B2 ");
    cube_move_apply_move_string(&cubes[104], "B1 R2 D2 B2 R2 U1 B2 L2 D3 B2 U1 F2 D2 F1 U3 R1 F2 D1 B2 R3 F2 ");
    cube_move_apply_move_string(&cubes[105], "F1 D3 B2 L3 R3 B2 L1 B2 F2 R2 U2 R3 F3 U2 R1 B3 R2 F1 L1 ");
    cube_move_apply_move_string(&cubes[106], "L2 B3 L1 F3 U1 R2 D1 F2 R2 B2 D2 R1 F2 L3 U2 R2 F2 R2 B1 R1 ");
    cube_move_apply_move_string(&cubes[107], "R1 B1 L2 F2 R2 U3 L2 F2 U2 L2 D1 U2 R3 B3 L3 F1 U3 L2 R2 ");
    cube_move_apply_move_string(&cubes[108], "L3 R2 D2 B3 L2 F3 R2 B1 F1 U2 D1 B2 F3 D3 F1 U3 L3 R2 ");
    cube_move_apply_move_string(&cubes[109], "U2 F3 D2 L2 F2 D2 B3 F2 L2 B2 D2 R3 F2 D1 F1 L1 F3 D3 F2 U1 ");
    cube_move_apply_move_string(&cubes[110], "L1 B1 F1 D2 R2 F3 D2 R2 U2 L2 B1 R2 F3 L3 B2 R2 D1 R3 U3 B1 R2 ");
    cube_move_apply_move_string(&cubes[111], "B1 R2 F1 U2 R2 D1 R3 U2 B1 D3 B2 L2 U1 B2 U1 B2 R2 F2 ");
    cube_move_apply_move_string(&cubes[112], "U2 L1 F2 R1 D2 R3 D2 B2 L3 U2 R2 F1 L1 B1 D3 F3 D2 F2 U3 B2 ");
    cube_move_apply_move_string(&cubes[113], "B3 R1 B1 U1 F2 U1 F2 D3 F2 D3 L2 U3 L3 F3 D2 R2 U3 R3 F2 ");
    cube_move_apply_move_string(&cubes[114], "D3 R2 B1 F2 L2 R2 U1 F3 R1 F3 U3 F3 D1 L3 R3 ");
    cube_move_apply_move_string(&cubes[115], "L2 B1 U2 F2 U3 B2 L2 D1 L2 B2 F2 D3 U3 R3 U1 L1 R3 U2 L3 F3 R2 ");
    cube_move_apply_move_string(&cubes[116], "R2 U1 R2 U3 B2 U1 L2 B2 U1 B2 L2 U3 F1 L3 B2 L1 U3 L3 D3 B3 ");
    cube_move_apply_move_string(&cubes[117], "U1 B2 L2 B2 L2 R2 D3 F2 L2 U1 R2 D1 R3 B2 U3 L3 B3 R2 B2 R2 ");
    cube_move_apply_move_string(&cubes[118], "B2 F2 L2 R2 U1 F2 U2 R2 U1 R2 B1 F1 U3 L2 D3 B1 R1 F2 ");
    cube_move_apply_move_string(&cubes[119], "U1 B3 R1 U2 D3 R1 U1 L2 F3 L2 D2 F1 U2 L2 B1 R2 L2 D2 B2 R3 ");
    cube_move_apply_move_string(&cubes[120], "D1 F1 U3 B2 U2 R2 B2 U3 L1 B1 R3 U2 R2 B3 U1 ");
    cube_move_apply_move_string(&cubes[121], "U2 D1 B3 U3 L1 U3 L3 F3 L2 F1 D2 F3 D2 L2 F1 U2 R2 F1 U2 R1 ");
    cube_move_apply_move_string(&cubes[122], "F3 L2 F2 D2 U2 L2 D2 L1 R2 B2 F1 D2 U3 F3 L3 R2 B1 R2 ");
    cube_move_apply_move_string(&cubes[123], "L2 R2 B1 R2 B1 F2 R2 D2 B1 F2 D2 F2 R3 U1 L3 F1 D1 L3 U1 R1 B1 ");
    cube_move_apply_move_string(&cubes[124], "L2 F3 D2 L2 F1 R2 U2 B1 U2 F3 U2 D1 B1 F1 L3 F1 L2 R1 F1 ");
    cube_move_apply_move_string(&cubes[125], "L2 R2 U3 L2 F2 D1 U3 F2 R2 D3 F1 U2 L3 U2 B2 U3 F3 L3 B3 U1 ");
    cube_move_apply_move_string(&cubes[126], "F2 R3 F3 R2 F3 U1 D2 R2 D1 F2 D3 R2 L2 F2 D2 L2 ");
    cube_move_apply_move_string(&cubes[127], "F3 D1 R2 F1 U2 L2 B1 F2 R2 F1 D2 F3 L2 U1 L1 B3 F2 D2 F2 D1 ");
    cube_move_apply_move_string(&cubes[128], "F1 R1 U2 F1 R3 B3 U1 F1 R1 D2 F1 R2 F3 B3 U2 L2 F3 R2 D2 R2 ");
    cube_move_apply_move_string(&cubes[129], "D2 L1 B2 D3 R2 B2 L2 D3 R1 D1 B1 U2 L1 B1 L2 ");
    cube_move_apply_move_string(&cubes[130], "R1 U2 B2 D1 F1 R1 D1 B3 U1 B2 L1 F2 R3 B2 U2 L1 U2 F2 D2 F2 ");
    cube_move_apply_move_string(&cubes[131], "L3 U1 L2 B3 D2 B2 L1 U2 F3 B3 R2 F1 R2 L2 F1 L2 D2 F2 L3 F3 ");
    cube_move_apply_move_string(&cubes[132], "L3 R2 F2 L2 D3 B2 F2 U3 R2 D2 F2 D3 B2 L1 D3 U2 L3 D2 U2 L3 ");
    cube_move_apply_move_string(&cubes[133], "L3 U3 D1 F1 L1 U3 B1 D2 L3 F2 U2 L2 U2 B2 U2 L3 U2 F3 D3 ");
    cube_move_apply_move_string(&cubes[134], "L3 U3 L1 F2 U2 L2 R1 B2 F2 U2 B2 R1 D2 F1 D3 L3 R1 U2 R3 D3 B3 ");
    cube_move_apply_move_string(&cubes[135], "B2 D3 L1 U3 F1 D3 B1 R1 B3 D2 B1 L2 B3 D2 F1 L2 F1 R2 L2 D3 ");
    cube_move_apply_move_string(&cubes[136], "L1 D2 B2 L3 R2 B2 L1 R2 B3 D3 L1 B2 U3 L1 D2 ");
    cube_move_apply_move_string(&cubes[137], "B3 R1 U3 B1 U3 F2 U1 R1 F3 D3 L2 F2 U2 F2 D1 ");
    cube_move_apply_move_string(&cubes[138], "D3 B2 L2 R2 U2 L2 F1 U2 F1 D2 L3 D2 F1 D2 U1 F3 D3 R3 U1 ");
    cube_move_apply_move_string(&cubes[139], "R1 U1 L2 U3 L3 F3 B3 R2 U1 L2 U3 F2 D3 B2 U1 F2 L2 D3 B3 U2 ");
    cube_move_apply_move_string(&cubes[140], "F3 U1 F3 D2 F1 U2 F2 R2 B1 L2 D2 U2 F2 U1 B1 L3 D2 F1 U1 F1 ");
    cube_move_apply_move_string(&cubes[141], "L3 U3 F3 U1 L2 U2 R3 F3 R2 B3 D2 F3 D2 R2 F3 R2 B2 R2 L3 ");
    cube_move_apply_move_string(&cubes[142], "B2 L2 D3 L2 D1 R2 B2 D2 F2 R2 D2 F3 L1 R3 D3 F2 L3 B3 F2 ");
    cube_move_apply_move_string(&cubes[143], "F3 R1 D2 F2 D2 R1 U2 L1 U2 L2 B2 R3 U2 F1 U3 F2 R2 F1 R1 D2 ");
    cube_move_apply_move_string(&cubes[144], "B1 R1 L1 U2 D1 F1 R3 D3 B1 F2 U1 L2 D2 F2 B2 ");
    cube_move_apply_move_string(&cubes[145], "D2 L2 U1 R2 U3 L2 D2 R2 U1 R2 D3 R2 L3 U1 F2 R3 B2 L2 B1 D2 ");
    cube_move_apply_move_string(&cubes[146], "U3 R3 F3 R2 B1 R3 B3 L3 B3 D1 F2 L2 F2 D2 R2 B2 D3 R2 L2 B2 ");
    cube_move_apply_move_string(&cubes[147], "B2 D2 F2 L1 R2 U2 R3 D2 U2 L1 U3 R3 F1 D1 B1 L1 D1 F2 R2 ");
    cube_move_apply_move_string(&cubes[148], "L3 D2 B2 D3 R2 D1 B2 L3 D2 R2 F3 L1 R3 F3 U2 ");
    cube_move_apply_move_string(&cubes[149], "L3 B2 R1 U2 L1 F1 U2 R1 F2 D1 F2 D1 R2 B2 U2 R2 U3 B2 D1 F2 ");
    cube_move_apply_move_string(&cubes[150], "U2 R2 B2 F2 D1 U2 F2 U1 L2 F2 R3 B2 D3 F1 D3 B2 L1 U1 B3 ");
    cube_move_apply_move_string(&cubes[151], "L3 F3 U2 F1 U2 L2 F3 U2 R2 D2 F2 U2 B1 U3 L2 U2 F3 D1 U3 L1 D2 ");
    cube_move_apply_move_string(&cubes[152], "R2 B3 U3 D2 L3 U1 F1 L2 U2 L1 U2 D2 R1 D3 L3 ");
    cube_move_apply_move_string(&cubes[153], "F3 D3 L2 D1 L2 B2 D1 R2 D2 B2 F1 L1 R3 B1 D2 R2 U1 B2 ");
    cube_move_apply_move_string(&cubes[154], "R1 F1 U1 F2 L3 B3 D3 R1 F3 U1 L2 F2 D1 L2 F2 R2 D1 L2 U3 F2 ");
    cube_move_apply_move_string(&cubes[155], "R2 F2 D2 R2 U1 R2 D3 B2 U2 B2 U2 B1 L1 B2 D1 L2 F1 U1 ");
    cube_move_apply_move_string(&cubes[156], "F1 D2 B2 U2 R2 B2 R2 U3 R2 U1 B1 D3 U3 L1 D2 F3 U3 R2 D1 ");
    cube_move_apply_move_string(&cubes[157], "R2 B1 U3 F2 R1 D3 F2 D1 F1 R1 U2 F2 L1 D2 B2 R2 B2 L1 D2 B2 L3 ");
    cube_move_apply_move_string(&cubes[158], "R1 D2 F2 L2 D2 L1 F2 U2 F2 D2 L2 U2 D1 L3 D2 U1 R1 U3 L2 ");
    cube_move_apply_move_string(&cubes[159], "U2 F2 R1 F2 U3 F1 R2 U2 D2 R2 B2 U3 R2 F3 L2 ");
    cube_move_apply_move_string(&cubes[160], "D3 F3 L2 B3 D2 R2 F1 R1 U1 R3 U3 R1 B1 L2 U1 ");
    cube_move_apply_move_string(&cubes[161], "B2 D1 F1 U2 D1 R3 B2 L1 R2 B3 D2 L2 F1 R2 U2 B3 U2 F2 D2 R3 ");
    cube_move_apply_move_string(&cubes[162], "D2 F1 L2 F2 U2 F1 R2 U2 F2 L2 U2 F3 U1 B1 L3 F1 D3 B2 F3 L3 D3 ");
    cube_move_apply_move_string(&cubes[163], "R3 B2 L1 D2 F2 L1 F2 U2 L1 B2 U2 F2 D3 B3 L1 F1 U2 L2 B2 D1 R3 ");
    cube_move_apply_move_string(&cubes[164], "R3 F3 L3 D1 R2 U3 R2 B1 R1 B2 L1 B2 U2 B2 D2 L3 F2 R3 U1 ");
    cube_move_apply_move_string(&cubes[165], "D2 B3 D2 B1 U2 B2 U2 R2 B2 F3 L2 F3 L3 R3 U3 L3 B3 U2 B1 L2 D2 ");
    cube_move_apply_move_string(&cubes[166], "R3 U1 F2 U2 R2 F1 R2 U2 R3 D3 F1 U1 B2 L3 R3 U2 ");
    cube_move_apply_move_string(&cubes[167], "B2 R2 U3 L2 B2 R2 F2 R2 B3 R3 D1 U1 F2 U1 B1 F1 D2 ");
    cube_move_apply_move_string(&cubes[168], "D3 B1 R1 B3 D1 F3 R1 U1 D1 F2 U3 R2 F2 R2 U1 R2 U1 R2 ");
    cube_move_apply_move_string(&cubes[169], "F3 U1 F3 D3 B3 D3 L3 U1 D2 B2 U3 F2 R2 U2 F2 D3 B2 D3 B3 ");
    cube_move_apply_move_string(&cubes[170], "U1 L2 F1 R1 U1 D3 R1 U1 F2 D3 R2 U1 L2 F1 D1 ");
    cube_move_apply_move_string(&cubes[171], "R1 B2 D2 L2 B2 U3 R2 B2 L2 F2 U1 R2 U3 F1 L1 B3 D2 U3 F1 R2 F1 ");
    cube_move_apply_move_string(&cubes[172], "B1 R3 D3 R1 D1 B3 L1 R2 D1 B2 R2 U2 L2 D3 B2 R2 B3 D2 ");
    cube_move_apply_move_string(&cubes[173], "U3 B1 D1 R2 U2 R2 U1 R2 F2 U1 L2 U1 R2 F3 L1 R3 F2 D3 F1 R2 U1 ");
    cube_move_apply_move_string(&cubes[174], "L1 U2 B2 U2 L1 B2 R3 U2 R2 U2 L2 F2 U3 B3 L2 D1 L1 B2 F2 D3 ");
    cube_move_apply_move_string(&cubes[175], "F2 D3 R1 B3 D2 F3 U3 L3 F1 L2 D2 B2 U2 D2 L2 F1 D2 B1 ");
    cube_move_apply_move_string(&cubes[176], "B2 U3 F2 L2 D2 U3 L2 U2 L2 F2 L2 R3 B3 D3 R2 B3 D1 F2 D3 F2 D1 ");
    cube_move_apply_move_string(&cubes[177], "L3 F3 D3 F2 L2 B2 U3 L2 R2 D1 B2 U1 L3 B2 U2 R1 U3 R3 U2 B3 ");
    cube_move_apply_move_string(&cubes[178], "R3 D3 B3 U1 R1 D1 B3 L2 B2 R2 L2 B1 D2 F3 D2 ");
    cube_move_apply_move_string(&cubes[179], "F2 U2 F2 D2 R3 U2 R2 D2 L1 B2 L3 F2 U3 L2 B2 U1 B3 D1 U2 B3 ");
    cube_move_apply_move_string(&cubes[180], "L2 U2 L2 R2 D3 F2 D1 L2 B1 D1 L3 R3 F3 R2 F3 U2 R2 U3 ");
    cube_move_apply_move_string(&cubes[181], "U2 D2 F2 B1 L3 D3 F2 R1 L2 F3 D2 R2 B3 L2 F1 D2 B1 D2 U3 ");
    cube_move_apply_move_string(&cubes[182], "F2 R3 D3 L3 D2 R3 B2 L3 F3 R2 D2 F2 B3 D2 F2 U2 L2 ");
    cube_move_apply_move_string(&cubes[183], "U3 R1 B2 U1 B2 F2 R2 D3 R2 B2 L2 U3 L3 D1 R2 U1 B1 U1 L2 ");
    cube_move_apply_move_string(&cubes[184], "B2 F2 D2 L1 F2 U2 L1 F2 L3 D2 L1 D1 U3 B3 L3 D3 L1 U1 B2 L2 ");
    cube_move_apply_move_string(&cubes[185], "R2 D1 F2 U2 F3 D2 F2 L2 F1 U2 F1 L2 U2 L1 F2 U3 B3 R2 B2 R1 B1 ");
    cube_move_apply_move_string(&cubes[186], "U2 F1 L1 D2 B3 L1 F2 R3 F3 L2 F2 R2 D1 B2 D3 B2 D1 F2 U1 B2 ");
    cube_move_apply_move_string(&cubes[187], "B2 R2 F1 L2 F1 L2 D2 B1 R2 U2 F1 L2 U3 L2 D1 L1 F3 R2 B3 L3 R1 ");
    cube_move_apply_move_string(&cubes[188], "U2 F1 L1 U1 L2 R2 D3 R2 D2 F2 D1 F1 L3 R3 U1 B3 L1 ");
    cube_move_apply_move_string(&cubes[189], "R1 U1 F2 R3 B3 D1 B1 L3 F3 L2 U2 R2 F2 L2 D1 B2 D3 R2 U2 L2 ");
    cube_move_apply_move_string(&cubes[190], "R1 U1 R2 D2 F1 U3 D1 B3 R1 D2 R2 D2 B2 L1 D2 L3 U2 B2 L3 D2 F1 ");
    cube_move_apply_move_string(&cubes[191], "U2 R3 U2 R3 F2 R2 U2 B2 F2 L1 F2 B3 U1 L1 U3 F2 U3 F3 R3 U2 ");
    cube_move_apply_move_string(&cubes[192], "B1 D3 R3 D2 R3 B1 D1 F1 B3 R2 B2 L2 F2 D2 B2 R1 F2 R3 L3 ");
    cube_move_apply_move_string(&cubes[193], "L2 F3 R2 U3 B1 L1 U2 R3 F3 U1 D2 L2 U1 R2 B2 D3 L2 U2 F2 ");
    cube_move_apply_move_string(&cubes[194], "F2 L1 B2 L3 F2 L2 D1 L1 D1 R1 B3 U3 R2 U3 L1 ");
    cube_move_apply_move_string(&cubes[195], "D1 B2 U2 B2 L2 B2 R3 B2 R1 D2 L3 D2 B2 F1 R1 U1 R1 B1 L1 D1 B1 ");
    cube_move_apply_move_string(&cubes[196], "L3 D2 F3 L2 D2 B2 L1 D3 L2 D2 B3 U2 R2 D2 R2 F3 D2 F2 B1 ");
    cube_move_apply_move_string(&cubes[197], "L2 F1 D3 L2 U3 R3 U2 F1 L3 F2 U2 F3 U2 B3 R2 F3 D2 B2 R2 U2 ");
    cube_move_apply_move_string(&cubes[198], "B1 D1 B1 D2 L2 F2 U2 L2 F2 D3 B2 R2 U2 F1 D3 B1 R3 U3 L3 U1 ");
    cube_move_apply_move_string(&cubes[199], "U3 L2 B2 U3 F2 D1 L2 D2 R2 B2 L1 B1 F3 R1 D3 R2 F1 R2 B3 U3 ");
    cube_move_apply_move_string(&cubes[200], "B3 L3 U2 R3 B2 L3 D2 R2 U2 R1 B2 D2 R1 B3 U3 R3 B3 U1 R2 D1 R1 ");
    cube_move_apply_move_string(&cubes[201], "U1 R1 L2 B3 L1 D3 L3 F2 D1 F3 U2 F1 L2 F1 L2 U2 L2 D2 B3 ");
    cube_move_apply_move_string(&cubes[202], "F2 R2 L3 F1 U2 F1 D3 R3 U3 F3 U2 B2 L2 B1 U2 B1 U2 D2 B1 D2 B1 ");
    cube_move_apply_move_string(&cubes[203], "R3 B2 U1 B2 D2 R2 U3 L2 D1 R2 D1 R2 L3 U1 L3 F3 L2 D1 B3 R1 ");
    cube_move_apply_move_string(&cubes[204], "L2 D2 R3 B2 R2 D2 B2 L1 B2 F2 D3 L1 F2 D3 B1 F3 U1 B3 L1 ");
    cube_move_apply_move_string(&cubes[205], "R1 D2 L2 F3 U2 B1 D2 L2 R2 F2 L2 D2 R1 F3 R3 U1 F3 R1 D2 B2 D2 ");
    cube_move_apply_move_string(&cubes[206], "R3 F1 U1 F2 B2 U1 L3 R2 U2 L2 F2 U1 F2 U2 L2 D3 R2 D1 R1 B2 ");
    cube_move_apply_move_string(&cubes[207], "R2 U2 B2 D2 U2 F3 U2 F3 L2 B3 R2 U2 D3 R3 D2 B3 R1 D3 B2 R3 ");
    cube_move_apply_move_string(&cubes[208], "U1 F3 L3 U1 B2 R2 U1 B2 D3 F2 L2 U1 F2 D2 B1 F3 D3 B3 U1 R3 U1 ");
    cube_move_apply_move_string(&cubes[209], "U3 L3 U1 F1 D1 R1 D3 F1 D2 R1 U2 R3 B2 L1 B2 R2 D2 L2 U2 ");
    cube_move_apply_move_string(&cubes[210], "F1 U2 B3 D1 B2 D3 R3 U1 L1 B2 L2 D2 L3 B2 R2 ");
    cube_move_apply_move_string(&cubes[211], "B3 L2 B3 U2 L2 B1 R2 F3 D2 B1 L2 R2 U3 L1 B2 F2 D3 B1 U2 B3 F1 ");
    cube_move_apply_move_string(&cubes[212], "B3 R3 D2 F3 L3 D3 R3 U1 B2 U1 R2 D3 F2 B2 L2 D1 R2 U1 F2 R3 ");
    cube_move_apply_move_string(&cubes[213], "U3 L3 F2 D1 F1 D3 F2 U1 R1 U1 R2 F2 B2 U2 L2 D1 F2 D3 F2 U3 B2 ");
    cube_move_apply_move_string(&cubes[214], "B2 R2 B2 F2 L1 D2 R3 B2 L1 U3 L3 B2 D3 F3 R3 F1 ");
    cube_move_apply_move_string(&cubes[215], "B3 D3 F2 U1 B2 L2 D3 L2 R2 D3 R2 D2 R3 B3 L2 D1 U2 L1 R3 B3 U2 ");
    cube_move_apply_move_string(&cubes[216], "D3 L1 D3 F1 U2 R3 B3 U2 F1 R2 U2 F2 D2 R2 F3 U2 B3 R2 D1 ");
    cube_move_apply_move_string(&cubes[217], "D1 R1 B3 D2 L3 F1 B2 D1 B1 R2 D2 F2 R2 B2 L2 U3 L2 D1 L2 B2 ");
    cube_move_apply_move_string(&cubes[218], "U2 R2 B2 R2 U3 F2 R2 D1 B2 L2 U1 B1 U3 L2 F2 D2 F3 R3 U1 L2 ");
    cube_move_apply_move_string(&cubes[219], "L3 D1 L2 B2 R2 U2 B2 U3 L2 U3 F2 U2 R1 U2 F3 L3 R2 D1 L2 ");
    cube_move_apply_move_string(&cubes[220], "F1 U1 B2 L1 F1 L2 D1 U2 F1 D2 R2 U2 F2 R2 B1 L2 F1 D2 U3 L2 ");
    cube_move_apply_move_string(&cubes[221], "R2 U1 F2 R2 F2 R2 F1 L2 D3 L3 R2 B3 U2 R3 B3 ");
    cube_move_apply_move_string(&cubes[222], "F1 U2 R1 B2 F2 R2 F2 R1 D2 R2 U3 B2 R3 D3 U3 B1 D3 ");
    cube_move_apply_move_string(&cubes[223], "L3 U3 R1 L1 B3 U3 R3 B3 R3 F2 U2 L2 U2 F2 U2 L3 B2 ");
    cube_move_apply_move_string(&cubes[224], "D2 L1 U1 R2 U2 B2 D3 B2 U3 B2 R2 U3 R2 B3 F3 R3 F1 L1 D1 F1 D1 ");
    cube_move_apply_move_string(&cubes[225], "F3 U1 B3 R3 D3 L2 F3 R3 B1 U2 R2 F2 R2 U1 F2 U1 B2 L2 F2 D1 ");
    cube_move_apply_move_string(&cubes[226], "U3 L2 F2 U1 F2 R2 U3 L2 D3 U3 R3 F3 R2 D1 R3 F2 D2 U3 R1 ");
    cube_move_apply_move_string(&cubes[227], "L3 R2 D1 R2 F2 D1 L2 B2 R2 U2 R2 F3 R3 F3 U2 L3 F2 D1 F3 D1 ");
    cube_move_apply_move_string(&cubes[228], "B2 D3 R2 B2 L2 U1 F2 D2 R2 F2 B1 L3 U2 F2 D3 L2 U1 B3 F1 L3 ");
    cube_move_apply_move_string(&cubes[229], "F1 L1 U2 B1 U3 B3 D3 B2 R3 U2 R1 U2 L2 B2 L2 F2 L1 U2 F3 ");
    cube_move_apply_move_string(&cubes[230], "D3 L3 F1 U2 F3 D2 U2 F1 L2 B3 L2 B2 U2 D1 B3 U1 B3 R2 F1 U1 ");
    cube_move_apply_move_string(&cubes[231], "B2 U1 L2 B2 U1 L2 U1 B2 R2 B2 U3 B3 U1 F2 L3 F1 R3 D2 F2 D1 ");
    cube_move_apply_move_string(&cubes[232], "F2 L3 U3 L2 U3 B2 L2 D3 B2 L2 U3 F2 D3 L2 B1 F2 D2 F3 R2 U3 R2 ");
    cube_move_apply_move_string(&cubes[233], "F1 D2 F1 D2 B2 R2 F3 R2 F3 R2 F1 U2 L3 R1 U1 L1 U2 L1 R2 D2 F3 ");
    cube_move_apply_move_string(&cubes[234], "R2 D3 R2 D3 B2 D1 L2 U3 B2 D2 F2 R3 B1 U2 R1 U1 R3 U1 ");
    cube_move_apply_move_string(&cubes[235], "D3 L1 D3 B2 D2 R2 D3 U2 R2 F3 D1 F3 L3 U2 R2 F3 U2 ");
    cube_move_apply_move_string(&cubes[236], "R3 D2 F3 R2 B2 U3 R2 F2 U1 R2 D1 U2 R1 B2 D2 B1 F3 R1 D3 ");
    cube_move_apply_move_string(&cubes[237], "U1 R2 F2 U2 R2 D3 F2 U1 R2 U2 F2 D3 L3 B1 L1 U3 F1 U3 F1 ");
    cube_move_apply_move_string(&cubes[238], "R3 D2 B2 R2 D1 L2 D1 U2 F2 R3 F1 L3 B1 D3 B3 ");
    cube_move_apply_move_string(&cubes[239], "R2 D3 L2 U1 B2 D2 U1 L2 U3 F2 L2 F2 B1 U1 L2 F3 D2 R2 D3 F1 ");
    cube_move_apply_move_string(&cubes[240], "D1 F2 D3 R2 B2 L2 U1 L2 R2 B2 L3 B2 F2 U3 L1 B3 L3 F2 D3 ");
    cube_move_apply_move_string(&cubes[241], "U2 B2 D2 R1 U2 R3 D2 L2 D2 B2 D2 B2 D3 F3 L2 B2 L3 F1 U1 F1 ");
    cube_move_apply_move_string(&cubes[242], "U3 B3 D2 B2 L2 R2 B1 D2 F1 D2 R2 D3 U3 B1 R2 U3 R1 D1 ");
    cube_move_apply_move_string(&cubes[243], "B1 R2 F3 L2 F1 L2 F3 D2 F2 L2 F1 D2 L1 U1 L2 U2 B2 U1 R3 B3 F3 ");
    cube_move_apply_move_string(&cubes[244], "B1 L2 F3 D2 B3 L2 R2 F2 R2 U2 F3 U2 L3 F3 L3 F2 D3 L2 R3 B1 D3 ");
    cube_move_apply_move_string(&cubes[245], "L3 D1 L2 B3 L2 U2 B1 U2 F1 L2 R2 F1 R2 F3 L1 D3 B1 R3 F1 R3 F2 ");
    cube_move_apply_move_string(&cubes[246], "D2 U2 F1 L2 F1 U2 F1 L2 R2 U2 R2 F2 R1 B2 U3 F1 L3 R2 U2 F3 ");
    cube_move_apply_move_string(&cubes[247], "R3 U2 R2 U1 R2 D3 L2 F2 U2 B2 D3 B2 D3 R1 B1 F2 U2 L2 F1 D3 B1 ");
    cube_move_apply_move_string(&cubes[248], "D1 R1 F2 L3 B2 D2 R1 U2 B2 R1 F2 D2 F2 U3 R3 D1 B3 L3 R2 ");
    cube_move_apply_move_string(&cubes[249], "L2 D1 B2 U3 L2 D2 R2 D1 R2 U3 B1 D3 L2 R3 B1 F2 U3 B1 ");
    cube_move_apply_move_string(&cubes[250], "L2 U2 B2 F2 U1 R2 F3 L3 R1 B3 L2 D2 B3 D2 R1 ");
    cube_move_apply_move_string(&cubes[251], "L1 U2 L3 D2 F1 U1 B1 R2 B2 U2 D2 R2 D2 R1 U2 D2 L1 U1 B3 ");
    cube_move_apply_move_string(&cubes[252], "L1 D1 F2 U2 L1 R1 F2 D2 R1 U2 L1 B2 R2 B1 L1 U2 F3 D2 F1 U3 ");
    cube_move_apply_move_string(&cubes[253], "D2 F1 D2 R2 U2 R2 B1 F2 R2 F2 D2 F1 R3 U3 R3 F3 L1 D3 F2 L3 R3 ");
    cube_move_apply_move_string(&cubes[254], "L2 D1 F2 R3 L3 D2 F1 D1 F2 D2 L3 B2 U2 R2 F2 ");
    cube_move_apply_move_string(&cubes[255], "B1 D1 R3 F2 U1 R3 U2 F1 B2 L1 B2 U2 F2 D2 R2 F2 L3 F2 L1 D2 ");
    cube_move_apply_move_string(&cubes[256], "R3 D3 B1 U3 B1 R3 F3 U1 B3 U2 R2 L1 U2 B2 R2 ");
    cube_move_apply_move_string(&cubes[257], "U2 R1 D1 L2 U3 B2 D1 F2 U1 L2 D2 R2 U3 B3 R3 F3 U3 F3 L2 U3 ");
    cube_move_apply_move_string(&cubes[258], "F3 U2 L1 F2 R1 D2 R3 D2 U2 L3 B2 D2 B2 D3 L1 R3 D1 U2 B3 U3 R2 ");
    cube_move_apply_move_string(&cubes[259], "F1 D2 F2 U1 B2 D1 B2 L2 F2 U1 B2 U2 R1 B1 D1 L3 U1 L1 D2 F3 ");
    cube_move_apply_move_string(&cubes[260], "L1 U1 F2 U1 F2 U2 L2 R2 F2 D3 R2 B3 R1 B3 F1 R2 D2 L1 R2 U3 ");
    cube_move_apply_move_string(&cubes[261], "B2 R1 D2 R3 D2 F2 L1 D2 L1 R2 U2 D3 F1 R3 B1 R2 D3 R1 D3 L1 ");
    cube_move_apply_move_string(&cubes[262], "U2 F1 U3 L2 U1 L2 F2 U1 F2 L2 D3 R2 F2 R1 D1 L2 B3 F2 U3 R1 ");
    cube_move_apply_move_string(&cubes[263], "U1 R1 F2 L2 F2 L3 B2 D2 L1 D2 L1 B2 D1 R3 F1 L1 F3 R3 D3 R1 ");
    cube_move_apply_move_string(&cubes[264], "F1 U1 R2 U2 L1 B3 U2 D2 R1 F2 U2 L2 F2 U3 F3 ");
    cube_move_apply_move_string(&cubes[265], "F3 D1 R1 U2 R2 D2 U2 B3 R2 B2 F3 L2 B2 F3 R3 U1 B1 U2 L2 D1 B2 ");
    cube_move_apply_move_string(&cubes[266], "D1 L2 R2 U1 L2 D1 R2 U2 L2 D3 U2 R1 U1 B1 F2 L1 F3 R1 F3 U3 L3 ");
    cube_move_apply_move_string(&cubes[267], "R2 U1 L2 R2 D2 B1 D2 F2 R2 F3 U2 B1 R1 F3 D3 R3 B1 D3 B2 ");
    cube_move_apply_move_string(&cubes[268], "U2 L2 U2 R2 U2 F3 L2 F1 L2 B2 D2 L3 U2 R1 D1 R1 B1 F3 L3 R3 ");
    cube_move_apply_move_string(&cubes[269], "L2 U3 B2 D2 U2 L2 F1 D2 L2 F3 U2 B2 D2 U2 L1 U3 F2 L2 B3 U3 L3 ");
    cube_move_apply_move_string(&cubes[270], "L3 U2 R2 B3 U2 F2 U2 L2 F1 U2 B3 L2 B1 U1 L3 F1 R1 B2 D2 F1 D2 ");
    cube_move_apply_move_string(&cubes[271], "B1 R3 F2 U2 B2 U3 L2 D1 R2 D3 B2 L1 U1 F3 D3 F3 D1 B1 U3 ");
    cube_move_apply_move_string(&cubes[272], "B3 L3 U2 L3 R2 D2 L3 R1 D3 B3 L1 R2 F3 L3 F2 ");
    cube_move_apply_move_string(&cubes[273], "F1 R3 D3 B2 L2 F2 U2 F2 U3 L2 B2 L2 B2 D1 R1 F3 U2 L2 F1 D1 R1 ");
    cube_move_apply_move_string(&cubes[274], "U1 R1 U1 B2 D1 R2 F2 D3 R2 U1 R2 U2 B2 R3 B3 U1 B2 D1 U2 B3 ");
    cube_move_apply_move_string(&cubes[275], "F2 U2 B3 R2 D2 L2 D2 B3 U2 F2 L2 R3 B2 D1 B3 U2 B3 L2 R1 U3 ");
    cube_move_apply_move_string(&cubes[276], "D2 R3 F1 R1 F3 U1 B3 L3 R2 D1 R2 U3 B2 R2 B2 U2 R2 U1 F2 D1 ");
    cube_move_apply_move_string(&cubes[277], "R1 F1 D2 R1 F1 U2 B1 U1 L2 U1 F2 D3 R2 D1 L2 U1 F2 L2 B1 L1 U1 ");
    cube_move_apply_move_string(&cubes[278], "B3 D1 B3 L2 U1 B3 D2 R3 U3 R1 B2 U2 D2 L3 B2 U2 F2 R1 D2 L3 ");
    cube_move_apply_move_string(&cubes[279], "D3 R2 U2 L2 B1 D3 L3 R2 D1 R2 B2 U1 B2 U3 B2 U2 L2 F2 D1 F3 ");
    cube_move_apply_move_string(&cubes[280], "F3 D3 B2 F2 L2 U3 B2 D2 U1 F2 D1 L3 D1 U1 L1 R1 B1 D3 L3 U1 ");
    cube_move_apply_move_string(&cubes[281], "R2 L1 F1 B3 L3 D2 R1 D3 R2 F2 U2 F3 U2 B1 R2 B2 R2 L2 F3 U1 ");
    cube_move_apply_move_string(&cubes[282], "U2 L2 D1 F2 D1 F2 U2 B2 R2 D1 L2 D2 F3 D3 L2 D3 R1 D1 B1 L1 B1 ");
    cube_move_apply_move_string(&cubes[283], "U2 R2 L1 U2 R1 F1 B1 R3 F3 U2 D1 F2 U3 R2 F2 B2 R2 D2 R2 ");
    cube_move_apply_move_string(&cubes[284], "B1 R1 L2 D1 R2 U3 F1 U2 D2 B2 R3 F2 B2 U2 D2 L3 D2 R1 U1 ");
    cube_move_apply_move_string(&cubes[285], "R3 B3 L1 U1 L2 U3 B2 U2 R2 F1 U3 F2 D2 R1 D3 ");
    cube_move_apply_move_string(&cubes[286], "L2 F1 U3 L1 U1 F1 U3 B1 D3 F2 R1 F2 R2 U2 R1 L2 B2 D2 B2 ");
    cube_move_apply_move_string(&cubes[287], "U1 B1 F2 D3 U2 L2 F2 D3 L1 U3 L3 R2 U2 L3 R1 ");
    cube_move_apply_move_string(&cubes[288], "L3 B2 U2 L2 R2 B2 F3 D2 B3 R2 F2 R1 D2 F1 R3 U1 L1 R1 U3 B3 ");
    cube_move_apply_move_string(&cubes[289], "U2 L3 B3 R3 B3 R2 F1 B3 R3 F2 D3 B2 D1 F2 U2 B2 R2 U2 D1 R2 L2 ");
    cube_move_apply_move_string(&cubes[290], "U1 L1 D3 F1 U3 R3 U2 F3 D3 F2 R2 F2 U3 R2 U3 L2 B2 L2 U1 R1 ");
    cube_move_apply_move_string(&cubes[291], "R2 U3 F2 D1 R3 F2 D1 B1 U1 D2 B2 U2 R3 D2 F2 B2 R3 U2 L3 U2 ");
    cube_move_apply_move_string(&cubes[292], "B3 R1 D2 B2 D2 R1 D2 U2 L3 U2 L1 B1 U3 L1 B2 L3 R3 D3 R2 ");
    cube_move_apply_move_string(&cubes[293], "B2 R2 F2 D3 F2 U3 L2 D2 U3 F2 B3 R1 D1 B3 U1 R1 B1 D1 L2 ");
    cube_move_apply_move_string(&cubes[294], "F1 D3 B2 L3 D1 B1 U1 L3 R2 U3 F2 D3 R2 B2 U2 R2 D3 L2 B2 U1 B3 ");
    cube_move_apply_move_string(&cubes[295], "L1 U1 B1 L1 B1 L3 F3 L3 B2 L2 D2 R2 D1 B2 D3 ");
    cube_move_apply_move_string(&cubes[296], "B3 R1 F2 R3 F2 R2 D2 R1 F2 R2 D2 F2 R3 F1 U1 L1 R2 B1 U3 B2 U1 ");
    cube_move_apply_move_string(&cubes[297], "U1 B3 R2 D3 B2 U2 L2 U1 F2 L2 B2 F2 D3 B2 L3 D3 R3 U1 L3 U3 R2 ");
    cube_move_apply_move_string(&cubes[298], "R2 U3 B2 U3 R2 F2 L2 U1 L2 U1 L3 U2 R2 B2 F1 L1 F2 U3 F2 ");
    cube_move_apply_move_string(&cubes[299], "L1 B3 U2 R2 F2 D3 L2 U1 L2 R2 U3 B2 L2 B1 L1 B3 L3 F1 R3 B2 ");
    cube_move_apply_move_string(&cubes[300], "R3 L2 F2 U1 L2 D1 L2 F2 U1 R2 U1 F1 L2 B2 U1 L3 B3 U1 F2 ");
    cube_move_apply_move_string(&cubes[301], "R3 U2 L2 R2 D3 L2 U1 R2 D3 R2 B2 R3 D2 F3 L1 R2 B1 R3 B1 ");
    cube_move_apply_move_string(&cubes[302], "D2 L1 B2 F2 R1 U2 F2 R3 F2 R3 B2 R3 F1 R1 F2 R2 D2 B3 ");
    cube_move_apply_move_string(&cubes[303], "F2 L2 F3 D2 R2 D2 U2 F1 L2 B2 U2 D3 L2 U3 B1 R1 D3 B1 L3 B2 U3 ");
    cube_move_apply_move_string(&cubes[304], "B2 R2 U1 F2 R2 D3 L2 F3 R3 U3 L1 D3 U1 F2 U2 ");
    cube_move_apply_move_string(&cubes[305], "F2 R1 L3 D2 B1 R3 U3 D2 R3 F2 R3 D2 R3 U2 F2 R2 U2 R2 B1 U3 L3 ");
    cube_move_apply_move_string(&cubes[306], "L3 F3 R1 U2 B2 R3 F2 U3 L3 R1 D3 B3 L2 D2 B2 ");
    cube_move_apply_move_string(&cubes[307], "R1 D1 R2 D1 R2 F2 D1 R2 D3 R2 B1 L3 B1 R1 D2 L3 U1 B3 D3 ");
    cube_move_apply_move_string(&cubes[308], "D1 F2 D3 R2 B2 U3 F2 R2 F2 D1 F2 R2 B3 D1 R1 F1 L1 B2 D1 L3 U3 ");
    cube_move_apply_move_string(&cubes[309], "F1 L2 F3 R2 U2 L2 U2 R2 D1 F2 D3 L1 D2 B3 R1 F2 L2 ");
    cube_move_apply_move_string(&cubes[310], "L3 U2 F2 D1 L2 R2 D3 B1 L1 D2 F1 D1 B3 U3 F1 ");
    cube_move_apply_move_string(&cubes[311], "F1 U1 R3 L2 B2 F2 D3 B2 L2 D2 L2 D3 R2 U1 L1 R3 F1 D3 F3 U1 R2 ");
    cube_move_apply_move_string(&cubes[312], "F1 B2 R1 B1 R3 F1 U2 F1 U1 R1 L3 U2 F2 L2 F2 R1 D2 L3 F2 U2 F2 ");
    cube_move_apply_move_string(&cubes[313], "U1 R2 D1 R2 U1 F2 D1 R2 B2 U1 R1 U1 B1 F1 R2 B1 D1 B2 R1 ");
    cube_move_apply_move_string(&cubes[314], "R2 U3 B1 D1 L2 R2 U2 F2 R2 D3 F2 U1 R2 D3 R3 F3 L3 D1 F3 L2 B3 ");
    cube_move_apply_move_string(&cubes[315], "U1 R3 B1 U2 L2 F3 L2 R2 B1 F2 D2 R2 B1 R1 D1 R3 D1 L2 U2 L3 ");
    cube_move_apply_move_string(&cubes[316], "B2 U1 F1 L3 F2 L1 B3 L3 F2 L3 D2 F2 B2 L3 U2 L2 B2 R2 F3 ");
    cube_move_apply_move_string(&cubes[317], "R1 B1 L2 B2 R2 U2 L2 D2 B1 L2 B1 D2 R2 D3 R3 B3 R2 D3 B1 L3 D2 ");
    cube_move_apply_move_string(&cubes[318], "D1 U2 R2 D3 L2 F2 D2 R2 F2 U3 L2 F2 R3 B1 F2 U2 B2 L3 D3 L1 ");
    cube_move_apply_move_string(&cubes[319], "R3 B3 R2 F1 L3 F1 U3 B1 R3 F2 B2 L3 U2 R2 B2 R1 U2 F2 B2 L3 ");
    cube_move_apply_move_string(&cubes[320], "L2 F3 U2 B1 L2 U2 L2 B1 F3 R2 F3 R3 U3 L3 B3 F1 D1 U1 F3 R2 ");
    cube_move_apply_move_string(&cubes[321], "L1 B1 D1 F2 R2 F1 D1 L3 R2 D2 B1 L2 F2 D2 L2 ");
    cube_move_apply_move_string(&cubes[322], "F1 U2 B2 L2 D2 R2 D3 F2 L2 U1 L2 B2 R1 B2 D3 U1 B1 D2 R3 F3 U2 ");
    cube_move_apply_move_string(&cubes[323], "B1 D2 R2 U2 L3 U2 L1 B2 L3 F2 R3 B2 R1 F3 D1 F2 D3 R3 B1 U2 ");
    cube_move_apply_move_string(&cubes[324], "B1 D2 F2 L1 F2 D1 B1 L2 U1 L1 D2 R1 B2 U2 L2 ");
    cube_move_apply_move_string(&cubes[325], "B2 R2 U1 B2 F2 R2 D3 U3 F2 D1 L3 B1 F2 R3 D3 R3 D3 F2 R3 F3 ");
    cube_move_apply_move_string(&cubes[326], "B3 L3 U2 F1 D1 B1 D3 B1 L1 B2 U2 F2 L1 U2 R2 F2 B2 D2 R1 U2 ");
    cube_move_apply_move_string(&cubes[327], "F1 U1 D2 R2 F1 U2 R2 B1 F2 R2 F1 D2 R2 L3 D2 B2 R1 F2 D2 U3 ");
    cube_move_apply_move_string(&cubes[328], "L1 U3 R1 L2 U1 F1 D1 R2 L1 D2 B2 R2 D2 L1 B2 L1 U2 L3 D2 F1 ");
    cube_move_apply_move_string(&cubes[329], "L3 D3 U2 B2 R1 D2 B2 F2 R3 U2 R2 U3 L3 R2 B1 U2 R1 B2 ");
    cube_move_apply_move_string(&cubes[330], "R2 D3 R2 U1 R2 B2 L2 B2 U3 L2 U1 B2 F3 U1 R1 F3 D1 U3 B1 L2 R3 ");
    cube_move_apply_move_string(&cubes[331], "U3 B1 U2 B2 U3 R2 B2 F2 U3 R2 U2 R1 D3 R2 B1 F1 D2 B3 ");
    cube_move_apply_move_string(&cubes[332], "F2 R3 U2 L3 D2 R1 B2 R1 F1 R2 U2 R1 U3 L1 U1 ");
    cube_move_apply_move_string(&cubes[333], "D2 F2 R2 B2 U2 B2 D3 R2 B2 U1 B1 D1 U1 B1 D1 L1 U1 R3 D1 ");
    cube_move_apply_move_string(&cubes[334], "F3 U1 F2 D2 U1 L2 F2 U2 R2 F2 U3 F1 D1 L3 U2 F2 U1 F2 L3 ");
    cube_move_apply_move_string(&cubes[335], "B1 D2 U3 B2 R2 U3 F2 U3 L2 D1 F2 U2 L3 U3 B2 F2 L1 R2 D1 ");
    cube_move_apply_move_string(&cubes[336], "L2 D1 L2 U1 F2 L2 U3 B2 R2 D1 B2 R3 D2 F1 D2 B3 U2 F2 D3 R1 B3 ");
    cube_move_apply_move_string(&cubes[337], "B3 R2 D2 R2 F2 L2 D3 L2 D2 R2 U1 L1 U1 R2 U2 R3 B1 D2 U1 L3 ");
    cube_move_apply_move_string(&cubes[338], "L2 F2 L1 U2 R1 B2 L1 D2 R2 U1 L1 F2 L3 B2 F3 ");
    cube_move_apply_move_string(&cubes[339], "R3 D1 R1 F2 R2 U2 R1 L2 D2 F3 L2 F1 R2 B3 U2 F2 R1 ");
    cube_move_apply_move_string(&cubes[340], "D3 R1 F1 B3 R1 F3 R1 U2 R1 L2 U1 D2 F2 D2 L2 D3 L2 ");
    cube_move_apply_move_string(&cubes[341], "R3 L2 F3 R2 U2 B1 L2 D2 L2 D2 U2 B1 L3 R3 D3 L2 B3 R3 B1 U3 ");
    cube_move_apply_move_string(&cubes[342], "R2 U2 F3 U1 F3 U3 F1 B2 R1 U2 R3 D2 F2 U2 F3 ");
    cube_move_apply_move_string(&cubes[343], "D2 F2 D1 L2 D1 R2 U3 L2 U3 R2 L1 B2 F2 U2 L3 D2 U1 F1 L2 ");
    cube_move_apply_move_string(&cubes[344], "B2 L2 D1 B2 R2 U3 L2 R2 D2 U3 L3 R1 D1 F2 R3 D1 L3 F3 D2 L3 ");
    cube_move_apply_move_string(&cubes[345], "D2 L2 U2 B2 L2 R1 B2 R2 D2 R3 B2 D3 L1 D2 R3 B3 R1 U1 L1 D2 ");
    cube_move_apply_move_string(&cubes[346], "U1 L3 B2 D1 F2 D1 R2 D1 U2 R2 U1 B2 L2 R1 U2 R3 D2 B1 L1 D3 ");
    cube_move_apply_move_string(&cubes[347], "F1 U3 L3 F1 B3 U2 F3 L1 U1 L2 D2 B2 R2 D2 F2 D3 R2 L2 U3 L1 ");
    cube_move_apply_move_string(&cubes[348], "F1 R3 F1 B1 D3 R3 D2 B1 L2 U1 B2 U1 D2 B2 U3 R2 D1 L2 F2 U1 ");
    cube_move_apply_move_string(&cubes[349], "U3 B2 L2 B2 D2 B2 R2 D3 R2 B2 U2 R3 U2 R3 F3 U3 B3 L3 B3 D1 R2 ");
    cube_move_apply_move_string(&cubes[350], "D1 B1 R3 F2 D2 R1 F1 R3 U1 D2 F3 D2 B3 L2 D2 F2 L2 D2 B3 ");
    cube_move_apply_move_string(&cubes[351], "L1 B3 U1 D3 F1 L1 D3 F2 B3 U3 R2 F2 U1 B2 L2 D3 B2 D1 R2 D3 L2 ");
    cube_move_apply_move_string(&cubes[352], "B3 L3 D1 B2 U3 B2 L2 B2 U2 B2 D1 B2 U1 R3 D2 R2 D3 B1 L1 U2 ");
    cube_move_apply_move_string(&cubes[353], "B3 D2 R2 U3 B2 U2 B2 U1 F2 U1 F2 U1 B2 L1 F3 D1 R3 U1 B2 R1 ");
    cube_move_apply_move_string(&cubes[354], "D3 B3 R1 F2 L3 F3 B2 R2 F1 D2 B1 U2 L2 D1 R2 ");
    cube_move_apply_move_string(&cubes[355], "L3 U2 D3 F1 R1 U3 R3 D1 B3 L3 D1 F2 U1 R2 F2 L2 D3 F2 L2 U3 L2 ");
    cube_move_apply_move_string(&cubes[356], "B1 U2 L2 B2 F2 R3 B2 L3 F2 R2 U1 L2 F2 R2 U2 R3 F1 ");
    cube_move_apply_move_string(&cubes[357], "B2 D1 U1 L2 R2 F2 L2 D3 R2 D2 U3 F1 U1 L3 B1 R2 U1 F1 D2 U1 ");
    cube_move_apply_move_string(&cubes[358], "B1 U3 D3 R3 U3 R3 B3 D3 L1 F2 D2 R2 L2 F2 B3 L2 F3 D2 B2 U2 ");
    cube_move_apply_move_string(&cubes[359], "D2 F3 B2 R1 U1 B1 R1 U2 B1 U2 F3 L2 B1 L2 U2 B2 D2 L3 B3 ");
    cube_move_apply_move_string(&cubes[360], "L1 B3 R2 U1 R2 F2 D3 R2 U1 B2 U2 B2 R2 F3 L2 D3 R3 D3 B1 L3 ");
    cube_move_apply_move_string(&cubes[361], "U3 L1 U3 L2 D1 F2 D1 R2 U2 L2 U2 B2 L2 F3 L3 R3 U1 B2 D1 R1 F3 ");
    cube_move_apply_move_string(&cubes[362], "U2 B2 R1 B2 D2 L2 B2 R1 B2 U2 R3 D2 F3 U1 B3 F3 U2 L1 F3 L2 U3 ");
    cube_move_apply_move_string(&cubes[363], "R1 F3 B1 R2 L1 U1 B2 R1 B2 R2 D1 F2 B2 D1 B2 U1 F2 B2 L2 ");
    cube_move_apply_move_string(&cubes[364], "R2 F2 U2 F3 L2 F3 R2 B1 L2 R2 B2 L3 B1 L2 D3 F3 U1 B2 R2 F2 ");
    cube_move_apply_move_string(&cubes[365], "R3 F1 U1 F1 B3 L1 B2 U3 F3 R1 D2 L3 D2 R1 U2 F2 L3 U2 D2 ");
    cube_move_apply_move_string(&cubes[366], "U3 L3 B2 F2 L3 D2 L1 D2 F2 R1 B2 U2 D1 R2 U1 B3 L3 U2 R1 B1 ");
    cube_move_apply_move_string(&cubes[367], "B1 L1 D2 R1 B2 L1 U2 L1 B2 F2 L1 D3 F3 R1 F2 R3 B2 U2 L2 ");
    cube_move_apply_move_string(&cubes[368], "D1 B2 D1 B2 D1 B2 L2 U1 R2 B2 R2 F1 U3 F1 U3 B3 U1 B3 L1 ");
    cube_move_apply_move_string(&cubes[369], "D1 F3 D2 L2 U2 F2 U2 F1 D2 B1 L2 D3 U3 R3 U2 B3 R2 B1 ");
    cube_move_apply_move_string(&cubes[370], "B1 U3 L2 B2 D2 R2 U2 F2 U2 R2 B1 F3 L3 D1 U3 R1 D3 L3 U2 R3 ");
    cube_move_apply_move_string(&cubes[371], "D1 L3 U2 F3 U3 L1 D3 F1 D3 B2 U2 B2 U1 R2 B2 R2 U1 R2 D1 B2 R1 ");
    cube_move_apply_move_string(&cubes[372], "U3 F1 U1 B2 F2 R2 D1 U3 F1 L1 B2 R2 B2 D1 R1 ");
    cube_move_apply_move_string(&cubes[373], "U3 B3 L2 R2 F1 L2 B1 L2 B3 D2 F3 D1 L1 F3 L1 B1 L3 D3 R1 ");
    cube_move_apply_move_string(&cubes[374], "L2 U3 F2 U2 B2 L2 D1 R2 B2 F2 D3 U3 L3 R2 F3 U2 L3 F3 R2 B2 U2 ");
    cube_move_apply_move_string(&cubes[375], "U3 L3 R2 D2 L2 F2 R3 B2 R1 B2 D2 B1 L2 B1 R1 D3 B1 R1 D1 U3 ");
    cube_move_apply_move_string(&cubes[376], "B2 D3 L2 B3 D2 L3 U1 L2 D3 F2 L1 F2 R1 D2 L1 B2 D2 R3 U2 D2 F2 ");
    cube_move_apply_move_string(&cubes[377], "D2 B2 U2 L2 R3 D2 L3 R3 U2 D1 R3 B2 D3 B3 U2 L3 R2 B3 ");
    cube_move_apply_move_string(&cubes[378], "D2 U2 B3 L2 F3 D2 B2 D2 F1 D2 F1 D2 R3 B3 D1 U2 F2 U3 L1 R3 B2 ");
    cube_move_apply_move_string(&cubes[379], "R1 U1 B2 U3 R2 U1 R2 D1 F2 U2 F2 U3 L3 F3 R2 U3 R1 U3 R1 B3 ");
    cube_move_apply_move_string(&cubes[380], "D2 L3 B2 L1 R2 D2 R1 B2 R1 F2 D2 U1 B3 R1 D2 U1 L3 R2 B1 F3 ");
    cube_move_apply_move_string(&cubes[381], "F3 U1 L3 B2 L2 B2 R2 D3 R2 B2 U2 B2 U1 L1 F1 L2 D2 F3 L2 B1 ");
    cube_move_apply_move_string(&cubes[382], "D2 B1 U1 R1 F2 U2 F3 D1 B2 D2 F3 R2 B1 R2 B3 U2 B1 ");
    cube_move_apply_move_string(&cubes[383], "D2 B2 L2 D2 U1 R2 U1 B1 U1 L3 U3 L1 U2 B3 R2 ");
    cube_move_apply_move_string(&cubes[384], "B3 U2 B3 L2 D2 B3 D2 B3 D2 F1 L2 D3 F1 D3 F3 R3 B1 D1 F1 L2 ");
    cube_move_apply_move_string(&cubes[385], "U2 B1 D2 F3 R2 F1 D2 B1 L2 F3 U2 R2 U3 R2 F3 R2 B2 U3 ");
    cube_move_apply_move_string(&cubes[386], "U2 L3 B2 L1 U3 F2 B1 D3 U2 R3 D2 R1 F2 U2 R1 D2 F2 B2 L1 U2 ");
    cube_move_apply_move_string(&cubes[387], "B1 U2 F2 R2 U2 B3 R2 U2 B3 R2 B1 U1 L1 D3 R1 D2 B3 U3 F3 U3 R3 ");
    cube_move_apply_move_string(&cubes[388], "D2 F1 D2 L3 F3 L1 F2 D1 L1 F2 U1 R2 D1 F2 D3 R2 D3 R2 U2 B2 ");
    cube_move_apply_move_string(&cubes[389], "F3 U1 R2 L2 B3 D1 B2 R1 B3 R2 U2 D1 L2 D3 B2 ");
    cube_move_apply_move_string(&cubes[390], "F2 R2 F2 D3 R2 D1 L2 D1 U1 B2 D3 L1 U1 L1 B2 R2 F3 U3 B1 R3 ");
    cube_move_apply_move_string(&cubes[391], "F3 L2 F1 L2 U2 L2 B3 D2 F2 U2 D3 R2 D2 R1 U1 F3 R2 ");
    cube_move_apply_move_string(&cubes[392], "F2 D3 R2 F2 L2 U3 B2 U3 R2 L3 F1 R3 U2 L3 F1 L3 R1 U2 ");
    cube_move_apply_move_string(&cubes[393], "L3 U3 F1 D2 L2 D1 B1 R1 U3 F3 L2 B1 U2 D2 B2 R2 D2 L2 B3 ");
    cube_move_apply_move_string(&cubes[394], "R3 B3 U3 L2 D1 L2 B2 U3 B2 R2 B2 L2 D1 L3 B3 R3 B3 L2 D1 L2 ");
    cube_move_apply_move_string(&cubes[395], "R2 D2 F3 U1 F1 L2 U2 B3 R3 D2 L2 B1 L2 D2 F1 ");
    cube_move_apply_move_string(&cubes[396], "U2 R3 U1 L2 U1 L2 B2 F2 D1 U2 F2 D1 R2 F2 B3 D1 R1 F1 R2 D1 U3 ");
    cube_move_apply_move_string(&cubes[397], "U2 L1 D1 R2 F2 D1 U1 B2 D3 L2 D1 B2 L2 U3 B3 U2 L3 D3 F3 L2 R1 ");
    cube_move_apply_move_string(&cubes[398], "B3 R2 B3 R2 U2 B2 R2 F3 L2 F3 D2 B3 R3 B3 D1 F1 D3 U2 B3 U1 ");
    cube_move_apply_move_string(&cubes[399], "U2 L1 U1 B2 D3 F2 L2 F2 L2 D2 L2 D3 L1 D1 U3 B1 F3 R3 D1 R2 ");
    cube_move_apply_move_string(&cubes[400], "F2 B3 R2 D3 L1 B1 L2 U3 B1 L2 F1 D2 F2 R2 U2 B1 L2 F2 D3 ");
    cube_move_apply_move_string(&cubes[401], "B2 U2 L3 D2 L1 F2 L3 D2 R1 D2 R3 B1 D1 B3 F3 L2 R1 D2 L3 F2 ");
    cube_move_apply_move_string(&cubes[402], "U2 D3 F3 B3 R1 L1 B3 U3 L1 R2 B3 L2 U2 F3 B1 D2 R2 F3 L2 U2 ");
    cube_move_apply_move_string(&cubes[403], "F1 B2 D1 L2 D1 U2 B2 D1 R2 F1 R3 U3 L2 B2 R2 U1 F1 ");
    cube_move_apply_move_string(&cubes[404], "L2 F2 U2 L3 F2 R1 F2 L2 D2 F2 U2 F1 D1 B2 L1 B3 R1 B2 R2 B2 ");
    cube_move_apply_move_string(&cubes[405], "U2 L2 F2 U2 R2 D3 B2 F2 D1 B2 R2 U2 L3 F3 R3 F3 D3 U1 F2 ");
    cube_move_apply_move_string(&cubes[406], "D3 B1 U2 B3 F2 D2 L2 F3 D3 L2 D3 U1 F3 R3 D2 ");
    cube_move_apply_move_string(&cubes[407], "B3 D3 L2 U3 L1 U1 B2 U1 L2 F3 D2 B2 D2 F2 R2 ");
    cube_move_apply_move_string(&cubes[408], "R1 B3 U2 F2 R3 U3 D3 L3 F1 R2 D2 F2 U2 B1 D2 R2 B3 R2 L3 ");
    cube_move_apply_move_string(&cubes[409], "L2 U1 B2 R2 U3 B2 R2 U3 F2 U1 R2 U3 R3 B1 U3 L3 R1 U3 L3 B2 ");
    cube_move_apply_move_string(&cubes[410], "R2 D1 B2 D2 L2 D1 B2 R2 B2 U1 L2 R1 U2 B1 R2 D2 B3 F2 R1 F3 ");
    cube_move_apply_move_string(&cubes[411], "R2 B2 R2 U2 L2 B3 D2 F3 R2 B1 L2 U3 L2 U2 F1 U2 R1 B3 D1 L1 ");
    cube_move_apply_move_string(&cubes[412], "D2 B2 D2 L2 U2 R3 B2 U2 R1 B2 L1 F2 U1 L3 D3 F3 R3 B1 D1 U3 R2 ");
    cube_move_apply_move_string(&cubes[413], "B1 D2 F3 D2 U2 L2 F3 U2 F2 R2 D2 F3 U3 F1 U2 F1 U3 F1 D3 F2 ");
    cube_move_apply_move_string(&cubes[414], "F2 D1 F1 U1 R1 U2 D2 B3 U3 D2 F2 B2 R1 L1 D2 R3 U2 B2 R1 D2 ");
    cube_move_apply_move_string(&cubes[415], "B2 D2 F2 D1 L2 B1 D3 U2 B2 F3 U3 R3 B1 D1 L1 ");
    cube_move_apply_move_string(&cubes[416], "L2 F3 D2 F2 U3 B2 D1 L2 D2 B2 D3 B2 U2 R3 D1 B1 L2 U3 F2 D3 ");
    cube_move_apply_move_string(&cubes[417], "L2 D3 B2 U3 R2 B2 U3 L2 U1 B2 D2 F3 D1 U1 F3 L1 F1 R3 D1 F1 ");
    cube_move_apply_move_string(&cubes[418], "U1 L2 U3 L2 D3 F2 D2 L2 B2 U1 B2 D3 R3 F3 U3 B2 D3 U2 L1 D2 U3 ");
    cube_move_apply_move_string(&cubes[419], "L1 B1 U3 R2 D1 U2 R2 U3 R1 B1 F3 R3 D1 B1 U2 ");
    cube_move_apply_move_string(&cubes[420], "R2 D1 L2 U1 F2 U2 F2 U3 B2 R2 B2 D3 F3 R1 F1 L1 B2 R3 D2 F2 U3 ");
    cube_move_apply_move_string(&cubes[421], "B3 U3 F1 D2 R2 F1 L2 B2 L2 F3 R1 B2 F3 L3 R1 U3 L2 ");
    cube_move_apply_move_string(&cubes[422], "B2 D3 L2 U3 B2 F2 U1 B2 U1 R3 B2 L3 B3 U3 L2 R2 F3 L3 ");
    cube_move_apply_move_string(&cubes[423], "R2 B3 U1 R2 U2 F1 U1 L3 D3 B2 U2 L1 F2 U2 R2 L3 D2 L1 ");
    cube_move_apply_move_string(&cubes[424], "L1 D1 F3 R2 B1 L1 D3 F2 D2 R3 F2 R2 D2 B2 R3 F2 R1 D1 B1 ");
    cube_move_apply_move_string(&cubes[425], "L2 F2 U2 R2 B1 D2 F1 R2 U2 R2 B3 D3 B2 L3 D1 L3 D2 U2 F3 ");
    cube_move_apply_move_string(&cubes[426], "U1 B2 D1 B2 U3 F2 U1 B2 D2 B2 L2 R3 B3 R3 D1 L1 B2 D2 B1 F3 D1 ");
    cube_move_apply_move_string(&cubes[427], "D2 B3 U2 F1 U2 F3 D2 B1 L2 U2 F2 D2 U1 L3 D1 R2 U3 B1 R2 U2 ");
    cube_move_apply_move_string(&cubes[428], "F1 B2 U3 R1 U3 R3 B3 L3 B3 D2 B2 R2 B2 L2 U2 B2 R2 U3 L2 D3 L2 ");
    cube_move_apply_move_string(&cubes[429], "F2 L2 U3 F2 U2 B2 R2 U1 R2 F2 R2 L1 F1 D1 B2 U1 R2 U3 B3 ");
    cube_move_apply_move_string(&cubes[430], "R1 D2 L3 R1 D2 F2 L1 D2 F1 R2 U3 B2 L1 B3 F2 L1 F2 ");
    cube_move_apply_move_string(&cubes[431], "F3 U2 L1 F2 B3 U3 F2 D1 B3 U1 D1 L2 U3 R2 B2 D2 L2 F2 B2 ");
    cube_move_apply_move_string(&cubes[432], "D2 U2 B2 F3 R2 F1 L2 D2 L2 F2 L2 B3 U1 R1 B3 D1 U2 L3 B1 F3 R1 ");









    int number_of_solutions = 1;
    int max_depth = 20;
    int verbose = false;
    int number_of_threads = nthreads;
    
    for (int i = 0; i < TEST_number_of_scrambles_solve_and_test_15_movers; i++){
        SolutionSet ss;
        solutionset_init(&ss, (size_t)number_of_solutions);

        cube_solvers_solve_cube(cubes[i], &ss, number_of_solutions, max_depth, verbose, number_of_threads, &fin);
        cr_assert_eq(ss.data->length, 15);
       
        solutionset_free(&ss);
    }
}



// Test(solver_fin, solver_15move_scrambles_correctly_1thread){
//     solve_and_test_15_movers(1);
// }


Test(solver_fin, solver_15move_scrambles_correctly_16thread){
    solve_and_test_15_movers(16);
}
