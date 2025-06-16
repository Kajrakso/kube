#include "bench.h"

/* Run 1_000_000 random moves and record the time it took. */
void bench_orig_moves();
void bench_orig_moves_and_index_calculation();
void bench_orig_moves_and_3_lookups_normal();
void bench_orig_moves_and_6_lookups_normal_and_inverse();
void bench_orig_moves_and_6_lookups_normal_and_inverse_new();


char* movers_16[19] = {
  "R2 L D' R2 L' B U' B' U' F' D' F2 B' D2 L' F",   "F' R2 U F2 L2 D' B2 U' R D B2 D2 F L B2 U'",
  "R2 L2 F R2 D2 F2 R2 F U R' D B2 D L F2 L",       "L' U2 D2 R' U' D' F2 U' F U F R2 B2 R B U2",
  "L' U2 R' F' U F B2 R L F' D' F2 B U2 R' B2",     "L2 F2 R2 U' D' R U B2 L B' D' F' B' R' B D2",
  "F' R B D2 R2 U' F' D F' D2 R2 B' F' D' F' U",    "F' D F2 B L' U' L' F' R U R' F' D' R' B2 L",
  "L U R2 U' D R U' L2 B2 D2 L2 B' L D F' R'",      "U R L' U2 D2 B' D2 L' F2 D' F2 R B' D' B2 L",
  "B D' B2 R U2 B U' L2 U B R2 F' D2 F D F'",       "U L B' L' D F B2 R' D' L' U2 D2 F L D F2",
  "D' B2 L' D2 B D' R U2 R' B2 L2 R U2 R' F2 L",    "R2 D B U2 L2 D' R2 D' R B R' U' R2 B' L2 F2",
  "R' B L2 D2 R F B U' B' L D F' D' U2 F L'",       "R2 F R' F2 U2 D' B' U2 L F' D F' D L' D2 F'",
  "B' D' L2 D' L' D2 R' F2 L2 B' R' F U2 L' R2 D2", "U2 R B D2 R D F' U' F' D B R D' U2 R' B2",
  "L U2 F2 U F U2 R2 D2 B' L U B' U F2 L2 B"};

char* movers_17[19] = {"R2 L D' R2 L' B U' B' U' F' D' F2 B' D2 L' F D'",
                       "F' R2 U F2 L2 D' B2 U' R D B2 D2 F L B2 U' F2",
                       "R2 L2 F R2 D2 F2 R2 F U R' D B2 D L F2 L F2",
                       "L' U2 D2 R' U' D' F2 U' F U F R2 B2 R B U2 D2",
                       "L' U2 R' F' U F B2 R L F' D' F2 B U2 R' B2 L",
                       "L2 F2 R2 U' D' R U B2 L B' D' F' B' R' B D2 L'",
                       "F' R B D2 R2 U' F' D F' D2 R2 B' F' D' F' U F",
                       "F' D F2 B L' U' L' F' R U R' F' D' R' B2 L R2",
                       "L U R2 U' D R U' L2 B2 D2 L2 B' L D F' R' F2",
                       "U R L' U2 D2 B' D2 L' F2 D' F2 R B' D' B2 L F'",
                       "B D' B2 R U2 B U' L2 U B R2 F' D2 F D F' R2",
                       "U L B' L' D F B2 R' D' L' U2 D2 F L D F2 R",
                       "D' B2 L' D2 B D' R U2 R' B2 L2 R U2 R' F2 L U'",
                       "R2 D B U2 L2 D' R2 D' R B R' U' R2 B' L2 F2 B'",
                       "R' B L2 D2 R F B U' B' L D F' D' U2 F L' F'",
                       "R2 F R' F2 U2 D' B' U2 L F' D F' D L' D2 F' R",
                       "B' D' L2 D' L' D2 R' F2 L2 B' R' F U2 L' R2 D2 F",
                       "U2 R B D2 R D F' U' F' D B R D' U2 R' B2 D",
                       "L U2 F2 U F U2 R2 D2 B' L U B' U F2 L2 B U2"};

char* movers_18[5] = {
  "D2 R2 L2 F U' R U F' B R F2 D2 B' R2 D L2 F U2", "R U' B2 L F2 B' L U F D F' R2 U2 R B' D R F'",
  "U2 D2 R' U' F' U' L2 D B R' D' F' L' U2 R2 D' L U2",
  "F2 U B' R' L' D R2 B L U R' B2 F D2 L2 B' U F", "U2 R D R F B' U R2 D2 B D' L R' D' F' R U R"};


void bench_orig_moves() {
    srand((unsigned int) time(NULL));
    clock_t start, end;
    cube_t  c = cube_scrambler_get_handscrambled_cube();
    int     moves[NUM_MOVES];

    for (size_t i = 0; i < NUM_MOVES; i++)
    {
        moves[i] = rand() % NMOVES;
    }

    // warm up
    for (size_t i = 0; i < NUM_MOVES; i++)
    {
        cube_move_apply_move(&c, moves[i]);
    }
    start = clock();
    for (size_t i = 0; i < NUM_MOVES; i++)
    {
        cube_move_apply_move(&c, moves[i]);
    }
    end = clock();

    printf("time for %lu moves: %f seconds\n", NUM_MOVES, (float) (end - start) / CLOCKS_PER_SEC);
}

void bench_orig_moves_and_index_calculation() {
    srand((unsigned int) time(NULL));
    clock_t start, end;
    cube_t  c = cube_scrambler_get_handscrambled_cube();
    int     moves[NUM_MOVES];

    for (size_t i = 0; i < NUM_MOVES; i++)
    {
        moves[i] = rand() % NMOVES;
    }

    // warmp up
    for (size_t i = 0; i < NUM_MOVES; i++)
    {
        cube_move_apply_move(&c, moves[i]);

        uint64_t p1 = cube_to_H_index(&c);

        cube_t   cube_z_rot = cube_operation_sym_conjugate(c, 7);
        uint64_t p2         = cube_to_H_index(&cube_z_rot);

        cube_t   cube_x_rot = cube_operation_sym_conjugate(c, 9);
        uint64_t p3         = cube_to_H_index(&cube_x_rot);
    }
    start = clock();
    for (size_t i = 0; i < NUM_MOVES; i++)
    {
        cube_move_apply_move(&c, moves[i]);

        uint64_t p1 = cube_to_H_index(&c);

        cube_t   cube_z_rot = cube_operation_sym_conjugate(c, 7);
        uint64_t p2         = cube_to_H_index(&cube_z_rot);

        cube_t   cube_x_rot = cube_operation_sym_conjugate(c, 9);
        uint64_t p3         = cube_to_H_index(&cube_x_rot);
    }
    end = clock();

    printf("time for %lu moves+3*index calculations: %f seconds\n", NUM_MOVES,
           (float) (end - start) / CLOCKS_PER_SEC);
}

void bench_orig_moves_and_3_lookups_normal() {
    srand((unsigned int) time(NULL));
    clock_t start, end;
    cube_t  c = cube_scrambler_get_handscrambled_cube();
    int     moves[NUM_MOVES];

    for (size_t i = 0; i < NUM_MOVES; i++)
    {
        moves[i] = rand() % NMOVES;
    }

    uint8_t* ptable = get_ptable_H();

    // warm up
    for (size_t i = 0; i < NUM_MOVES; i++)
    {
        cube_move_apply_move(&c, moves[i]);

        uint64_t p1 = cube_to_H_index(&c);
        ptable_read_val(p1, ptable);

        cube_t   cube_z_rot = cube_operation_sym_conjugate(c, 7);
        uint64_t p2         = cube_to_H_index(&cube_z_rot);
        ptable_read_val(p2, ptable);

        cube_t   cube_x_rot = cube_operation_sym_conjugate(c, 9);
        uint64_t p3         = cube_to_H_index(&cube_x_rot);
        ptable_read_val(p3, ptable);
    }
    start = clock();
    for (size_t i = 0; i < NUM_MOVES; i++)
    {
        cube_move_apply_move(&c, moves[i]);

        uint64_t p1 = cube_to_H_index(&c);
        ptable_read_val(p1, ptable);

        cube_t   cube_z_rot = cube_operation_sym_conjugate(c, 7);
        uint64_t p2         = cube_to_H_index(&cube_z_rot);
        ptable_read_val(p2, ptable);

        cube_t   cube_x_rot = cube_operation_sym_conjugate(c, 9);
        uint64_t p3         = cube_to_H_index(&cube_x_rot);
        ptable_read_val(p3, ptable);
    }
    end = clock();

    printf("time for %lu moves+3 lookups normal: %f seconds\n", NUM_MOVES,
           (float) (end - start) / CLOCKS_PER_SEC);
}

void bench_orig_moves_and_6_lookups_normal_and_inverse() {
    srand((unsigned int) time(NULL));
    clock_t start, end;
    cube_t  c = cube_scrambler_get_handscrambled_cube();
    int     moves[NUM_MOVES];

    for (size_t i = 0; i < NUM_MOVES; i++)
    {
        moves[i] = rand() % NMOVES;
    }

    uint8_t* ptable = get_ptable_H();
    // warm up
    for (size_t i = 0; i < NUM_MOVES; i++)
    {
        cube_move_apply_move(&c, moves[i]);
        uint64_t p1 = cube_to_H_index(&c);
        ptable_read_val(p1, ptable);

        cube_t   cube_z_rot = cube_operation_sym_conjugate(c, 7);
        uint64_t p2         = cube_to_H_index(&cube_z_rot);
        ptable_read_val(p2, ptable);

        cube_t   cube_x_rot = cube_operation_sym_conjugate(c, 9);
        uint64_t p3         = cube_to_H_index(&cube_x_rot);
        ptable_read_val(p3, ptable);


        cube_t inv = cube_operation_inverse(c);

        uint64_t      p1_inv      = cube_to_H_index(&inv);
        const uint8_t pval_UD_inv = ptable_read_val(p1_inv, ptable);

        cube_t        cube_z_rot_inv = cube_operation_sym_conjugate(inv, 7);
        uint64_t      p2_inv         = cube_to_H_index(&cube_z_rot_inv);
        const uint8_t pval_LR_inv    = ptable_read_val(p2_inv, ptable);

        cube_t        cube_x_rot_inv = cube_operation_sym_conjugate(inv, 9);
        uint64_t      p3_inv         = cube_to_H_index(&cube_x_rot_inv);
        const uint8_t pval_FB_inv    = ptable_read_val(p3_inv, ptable);
    }
    start = clock();
    for (size_t i = 0; i < NUM_MOVES; i++)
    {
        cube_move_apply_move(&c, moves[i]);
        uint64_t p1 = cube_to_H_index(&c);
        ptable_read_val(p1, ptable);

        cube_t   cube_z_rot = cube_operation_sym_conjugate(c, 7);
        uint64_t p2         = cube_to_H_index(&cube_z_rot);
        ptable_read_val(p2, ptable);

        cube_t   cube_x_rot = cube_operation_sym_conjugate(c, 9);
        uint64_t p3         = cube_to_H_index(&cube_x_rot);
        ptable_read_val(p3, ptable);


        cube_t inv = cube_operation_inverse(c);

        uint64_t      p1_inv      = cube_to_H_index(&inv);
        const uint8_t pval_UD_inv = ptable_read_val(p1_inv, ptable);

        cube_t        cube_z_rot_inv = cube_operation_sym_conjugate(inv, 7);
        uint64_t      p2_inv         = cube_to_H_index(&cube_z_rot_inv);
        const uint8_t pval_LR_inv    = ptable_read_val(p2_inv, ptable);

        cube_t        cube_x_rot_inv = cube_operation_sym_conjugate(inv, 9);
        uint64_t      p3_inv         = cube_to_H_index(&cube_x_rot_inv);
        const uint8_t pval_FB_inv    = ptable_read_val(p3_inv, ptable);
    }
    end = clock();

    printf("time for %lu moves+6 lookups normal and inverse: %f seconds\n", NUM_MOVES,
           (float) (end - start) / CLOCKS_PER_SEC);
}


void bench_orig_moves_and_6_lookups_normal_and_inverse_new() {
    srand((unsigned int) time(NULL));
    clock_t start, end;
    cube_t  c = cube_scrambler_get_handscrambled_cube();
    int     moves[NUM_MOVES];

    for (size_t i = 0; i < NUM_MOVES; i++)
    {
        moves[i] = rand() % NMOVES;
    }

    uint8_t* ptable = get_ptable_H();
    // warm up
    for (size_t i = 0; i < NUM_MOVES; i++)
    {
        cube_move_apply_move(&c, moves[i]);
        uint64_t p1 = cube_to_H_index(&c);
        ptable_read_val(p1, ptable);

        uint64_t p2 = cube_to_H_lr_index(&c);
        ptable_read_val(p2, ptable);

        uint64_t p3 = cube_to_H_fb_index(&c);
        ptable_read_val(p3, ptable);

        cube_t inv = cube_operation_inverse(c);

        uint64_t      p1_inv      = cube_to_H_index(&inv);
        const uint8_t pval_UD_inv = ptable_read_val(p1_inv, ptable);

        uint64_t      p2_inv      = cube_to_H_lr_index(&inv);
        const uint8_t pval_LR_inv = ptable_read_val(p2_inv, ptable);

        uint64_t      p3_inv      = cube_to_H_fb_index(&inv);
        const uint8_t pval_FB_inv = ptable_read_val(p3_inv, ptable);
    }
    start = clock();
    for (size_t i = 0; i < NUM_MOVES; i++)
    {
        cube_move_apply_move(&c, moves[i]);
        uint64_t p1 = cube_to_H_index(&c);
        ptable_read_val(p1, ptable);

        uint64_t p2 = cube_to_H_lr_index(&c);
        ptable_read_val(p2, ptable);

        uint64_t p3 = cube_to_H_fb_index(&c);
        ptable_read_val(p3, ptable);

        cube_t inv = cube_operation_inverse(c);

        uint64_t      p1_inv      = cube_to_H_index(&inv);
        const uint8_t pval_UD_inv = ptable_read_val(p1_inv, ptable);

        uint64_t      p2_inv      = cube_to_H_lr_index(&inv);
        const uint8_t pval_LR_inv = ptable_read_val(p2_inv, ptable);

        uint64_t      p3_inv      = cube_to_H_fb_index(&inv);
        const uint8_t pval_FB_inv = ptable_read_val(p3_inv, ptable);
    }
    end = clock();

    printf("time for %lu moves+6 lookups normal and inverse (new): %f seconds\n", NUM_MOVES,
           (float) (end - start) / CLOCKS_PER_SEC);
}


void bench() {
    cube_tables_generate();
    cube_tables_load();

    bench_orig_moves();
    bench_orig_moves();
    bench_orig_moves();
    bench_orig_moves_and_index_calculation();
    bench_orig_moves_and_index_calculation();
    bench_orig_moves_and_index_calculation();
    bench_orig_moves_and_3_lookups_normal();
    bench_orig_moves_and_3_lookups_normal();
    bench_orig_moves_and_3_lookups_normal();
    bench_orig_moves_and_6_lookups_normal_and_inverse();
    bench_orig_moves_and_6_lookups_normal_and_inverse();
    bench_orig_moves_and_6_lookups_normal_and_inverse();
    bench_orig_moves_and_6_lookups_normal_and_inverse_new();
    bench_orig_moves_and_6_lookups_normal_and_inverse_new();
    bench_orig_moves_and_6_lookups_normal_and_inverse_new();

    cube_tables_free();
}

void bench_solver() {
    cube_tables_generate();
    cube_tables_load();
    for (int i = 0; i < 19; i++)
    {
        // char* scr = movers_16[i];
        char* scr = movers_17[i];
        // char* scr = movers_18[i];

        cube_t cube = cube_create_new_cube();
        cube_move_apply_move_string(&cube, scr);
        printf("Scramble: %s\n\n", scr);

        const int num_sols = 1;
        // prepare array  for solutions
        int* solutions = malloc(sizeof(int) * 20 * num_sols);

        bool solved = cube_solvers_solve_cube(cube, solutions, num_sols);

        if (!solved)
            printf("Was not able to solve the cube :(\n");
        else
            cube_print_solutions(solutions, num_sols);

        free(solutions);
    }
    cube_tables_free();
}
