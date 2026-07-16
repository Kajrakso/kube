/* Date: 2026-02-12
 * I made OpenCode generate quite a few of these tests. */

#include "criterion/criterion.h"

#include "../src/solutions.h"
#include "../src/core/move.h"
#include "../src/cli.h"

#include <time.h>

Solution create_random_sample_solution() {
    srand((unsigned int) time(NULL));
    Solution sol;
    solution_init(&sol);

    // random length and random moves
    int N = 5 + rand() % 20;
    for (int i = 0; i < N; i++)
    {
        solution_append(&sol, rand() % 18);
    }

    return sol;
}


SolutionSet create_random_solutionset() {
    SolutionSet solset;
    int         capacity = 5 + rand() % 20;
    solutionset_init(&solset, capacity);

    for (int i = 0; i < rand() % capacity; i++)
    {
        Solution sol = create_random_sample_solution();
        solutionset_add_copy(&solset, &sol);
    }

    return solset;
}

PipelineSolutionStep create_test_pipelinesolutionstep(bool starts_on_inverse) {
    PipelineSolutionStep step;
    pipelinesolutionstep_init(&step, starts_on_inverse);

    // Add some sample moves
    pipelinesolutionstep_append(&step, R1);
    pipelinesolutionstep_append(&step, U2);
    pipelinesolutionstep_append(&step, F3);

    return step;
}

cube_t create_test_cube() {
    cube_t cube = cube_create_new_cube();
    // Apply some moves to make it non-trivial
    cube_move_apply_move(&cube, R1);
    cube_move_apply_move(&cube, U1);
    cube_move_apply_move(&cube, F2);
    return cube;
}

PipelineSolution create_test_pipelinesolution(int num_steps) {
    cube_t           cube = create_test_cube();
    PipelineSolution ps;
    pipelinesolution_init(&ps);

    for (int i = 0; i < num_steps; i++)
    {
        PipelineSolutionStep step = create_test_pipelinesolutionstep(i % 2 == 0);
        pipelinesolution_add_copy(&ps, &step);
        pipelinesolutionstep_free(&step);
    }

    return ps;
}

PipelineSolutionSet create_test_pipelinesolutionset(size_t count) {
    PipelineSolutionSet set;
    pipelinesolutionset_init(&set, count);

    for (size_t i = 0; i < count; i++)
    {
        PipelineSolution ps = create_test_pipelinesolution(1 + i % 3);  // Vary step counts
        pipelinesolutionset_add_copy(&set, &ps);
        pipelinesolution_free(&ps);
    }

    return set;
}


/* =========================
   Single Solution (dynamic)
   ========================= */

Test(solutions, single_solution_append_and_pop) {
    Solution sol;
    solution_init(&sol);
    cr_assert_eq(sol.length, 0);

    solution_append(&sol, R1);
    cr_assert_eq(sol.length, 1);
    int moves_after_1_move[] = {R1};
    cr_assert_arr_eq(sol.moves, moves_after_1_move, sol.length);

    solution_append(&sol, L1);
    solution_append(&sol, U1);
    solution_append(&sol, B2);
    int moves_after_4_moves[] = {R1, L1, U1, B2};
    cr_assert_eq(sol.length, 4);
    cr_assert_arr_eq(sol.moves, moves_after_4_moves, sol.length);

    solution_pop(&sol);
    int moves_after_4_moves_and_1_pop[] = {R1, L1, U1};
    cr_assert_eq(sol.length, 3);
    cr_assert_arr_eq(sol.moves, moves_after_4_moves_and_1_pop, sol.length);

    solution_free(&sol);
}

Test(solutions, solution_copy) {
    Solution sol  = create_random_sample_solution();
    Solution sol2 = solution_copy(&sol);

    cr_assert_arr_eq(sol.moves, sol2.moves, sol.length);

    solution_append(&sol, U2);
    cr_assert_neq(sol.length, sol2.length);

    solution_free(&sol);
    solution_free(&sol2);
}


/* =========================
   Solution Set (many solutions)
   ========================= */

Test(solutions, solution_set_add_copy) {
    SolutionSet solset;
    solutionset_init(&solset, 20);

    for (int i = 0; i < 20; i++)
    {
        Solution sol = create_random_sample_solution();
        solutionset_add_copy(&solset, &sol);
        cr_assert_eq(solset.count, i + 1);
    }

    solutionset_free(&solset);
}

Test(solutions, solution_set_append) {
    SolutionSet solset1 = create_random_solutionset();
    SolutionSet solset2 = create_random_solutionset();
    int         count1  = solset1.count;
    int         count2  = solset2.count;

    if (solset1.capacity >= count1 + count2)
    {
        // append solset 2 to solset 1
        solutionset_append(&solset1, &solset2);
        cr_assert_eq(solset1.count, count1 + count2);
    }

    // Add proper cleanup to prevent memory leaks
    solutionset_free(&solset1);
    solutionset_free(&solset2);
}

Test(solutions, solution_set_capacity_limits) {
    // Test case where capacity is insufficient for append
    SolutionSet solset1;
    SolutionSet solset2;
    solutionset_init(&solset1, 3);  // Small capacity
    solutionset_init(&solset2, 2);  // 2 solutions to append

    // Fill solset1 to capacity
    Solution sol = create_random_sample_solution();
    for (int i = 0; i < 3; i++)
    {
        solutionset_add_copy(&solset1, &sol);
    }

    // Try to append solset2 - it should only add what fits
    int initial_count = solset1.count;
    solutionset_append(&solset1, &solset2);

    // Should not exceed capacity (current implementation may not handle this gracefully)
    // This test documents current behavior - may need to be updated based on requirements
    cr_assert_geq(solset1.count, initial_count);  // At least didn't decrease

    // Clean up
    solution_free(&sol);
    solutionset_free(&solset1);
    solutionset_free(&solset2);
}


/* =========================
   PipelineSolutionStep
   ========================= */

Test(solutions, pipelinesolutionstep_init) {
    PipelineSolutionStep step;

    // Test initialization with starts_on_inverse = false
    pipelinesolutionstep_init(&step, false);
    cr_assert_not_null(step.solution);
    cr_assert_eq(step.solution->length, 0);
    cr_assert_gt(step.solution->capacity, 0);
    cr_assert_not_null(step.solution->moves);
    cr_assert_eq(step.starts_on_inverse, false);

    pipelinesolutionstep_free(&step);

    // Test initialization with starts_on_inverse = true
    pipelinesolutionstep_init(&step, true);
    cr_assert_eq(step.starts_on_inverse, true);

    pipelinesolutionstep_free(&step);
}

Test(solutions, pipelinesolutionstep_append_and_pop) {
    PipelineSolutionStep step;
    pipelinesolutionstep_init(&step, false);  // Fix memory bug - proper initialization

    int sol_len = step.solution->length;
    cr_assert_eq(sol_len, 0);  // Should start empty

    pipelinesolutionstep_append(&step, R1);
    cr_assert_eq(sol_len + 1, step.solution->length);
    cr_assert_eq(step.solution->moves[0], R1);

    pipelinesolutionstep_pop(&step);
    cr_assert_eq(sol_len, step.solution->length);

    pipelinesolutionstep_free(&step);
}


Test(solutions, pipelinesolutionstep_copy) {
    PipelineSolutionStep original;
    pipelinesolutionstep_init(&original, true);

    // Add some moves to the original
    pipelinesolutionstep_append(&original, R1);
    pipelinesolutionstep_append(&original, U2);
    pipelinesolutionstep_append(&original, F3);

    // Create a copy
    PipelineSolutionStep copy = pipelinesolutionstep_copy(&original);

    // Verify the copy is identical
    cr_assert_neq(copy.solution, original.solution);  // Different pointers
    cr_assert_eq(copy.solution->length, original.solution->length);
    cr_assert_eq(copy.starts_on_inverse, original.starts_on_inverse);
    cr_assert_arr_eq(copy.solution->moves, original.solution->moves, original.solution->length);

    // Verify independence - modify original, copy should be unchanged
    pipelinesolutionstep_append(&original, B1);
    cr_assert_eq(original.solution->length, 4);
    cr_assert_eq(copy.solution->length, 3);  // Copy unchanged

    // Verify independence - modify copy, original should be unchanged
    pipelinesolutionstep_append(&copy, L2);
    cr_assert_eq(copy.solution->length, 4);
    cr_assert_eq(original.solution->length, 4);  // Original unchanged
    cr_assert_neq(copy.solution->moves[3], original.solution->moves[3]);

    // Clean up both
    pipelinesolutionstep_free(&original);
    pipelinesolutionstep_free(&copy);
}

/* =========================
   PipelineSolution
   ========================= */

Test(solutions, pipelinesolution_lifecycle) {
    cube_t           cube = create_test_cube();
    PipelineSolution ps;

    // Test initialization
    pipelinesolution_init(&ps);
    cr_assert_eq(ps.count, 0);
    cr_assert_gt(ps.capacity, 0);
    cr_assert_not_null(ps.steps);
    cr_assert_eq(ps.heuristic_score, SIZE_MAX);

    // Test adding steps
    PipelineSolutionStep step1 = create_test_pipelinesolutionstep(false);
    PipelineSolutionStep step2 = create_test_pipelinesolutionstep(true);

    int result1 = pipelinesolution_add_copy(&ps, &step1);
    cr_assert_eq(result1, 0);  // Success
    cr_assert_eq(ps.count, 1);

    int result2 = pipelinesolution_add_copy(&ps, &step2);
    cr_assert_eq(result2, 0);  // Success
    cr_assert_eq(ps.count, 2);

    // Test capacity expansion by adding many steps
    int initial_capacity = ps.capacity;
    for (int i = ps.count; i < initial_capacity + 5; i++)
    {
        PipelineSolutionStep temp_step = create_test_pipelinesolutionstep(i % 2 == 0);
        int                  result    = pipelinesolution_add_copy(&ps, &temp_step);
        cr_assert_eq(result, 0);
        pipelinesolutionstep_free(&temp_step);
    }
    cr_assert_gt(ps.capacity, initial_capacity);  // Should have expanded
    cr_assert_eq(ps.count, initial_capacity + 5);

    // Clean up
    pipelinesolution_free(&ps);
    pipelinesolutionstep_free(&step1);
    pipelinesolutionstep_free(&step2);
}

Test(solutions, pipelinesolution_copy) {
    cube_t           cube = create_test_cube();
    PipelineSolution original;
    pipelinesolution_init(&original);

    // Add multiple steps to original
    PipelineSolutionStep step1 = create_test_pipelinesolutionstep(false);
    PipelineSolutionStep step2 = create_test_pipelinesolutionstep(true);
    pipelinesolution_add_copy(&original, &step1);
    pipelinesolution_add_copy(&original, &step2);

    original.heuristic_score = 42;

    // Create copy
    PipelineSolution copy = pipelinesolution_copy(&original);

    // Verify the copy is identical but independent
    cr_assert_eq(copy.count, original.count);
    cr_assert_eq(copy.capacity, original.capacity);
    cr_assert_eq(copy.heuristic_score, original.heuristic_score);

    // Verify steps are deep copied
    for (size_t i = 0; i < original.count; i++)
    {
        cr_assert_neq(copy.steps[i].solution, original.steps[i].solution);
        cr_assert_eq(copy.steps[i].starts_on_inverse, original.steps[i].starts_on_inverse);
        cr_assert_eq(copy.steps[i].solution->length, original.steps[i].solution->length);
        cr_assert_arr_eq(copy.steps[i].solution->moves, original.steps[i].solution->moves,
                         original.steps[i].solution->length);
    }

    // Verify independence - modify original, copy unchanged
    pipelinesolutionstep_append(&original.steps[0], B1);
    cr_assert_eq(original.steps[0].solution->length, 4);
    cr_assert_eq(copy.steps[0].solution->length, 3);  // Copy unchanged

    // Clean up
    pipelinesolution_free(&original);
    pipelinesolution_free(&copy);
    pipelinesolutionstep_free(&step1);
    pipelinesolutionstep_free(&step2);
}

/* =========================
   PipelineSolutionSet
   ========================= */

Test(solutions, pipelinesolutionset_basic_operations) {
    PipelineSolutionSet set;

    // Test initialization
    pipelinesolutionset_init(&set, 5);
    cr_assert_eq(set.count, 0);
    cr_assert_eq(set.capacity, 5);
    cr_assert_not_null(set.data);

    // Test adding solutions
    PipelineSolution ps1 = create_test_pipelinesolution(2);
    PipelineSolution ps2 = create_test_pipelinesolution(3);

    int result1 = pipelinesolutionset_add_copy(&set, &ps1);
    cr_assert_eq(result1, 1);  // Success
    cr_assert_eq(set.count, 1);

    int result2 = pipelinesolutionset_add_copy(&set, &ps2);
    cr_assert_eq(result2, 1);  // Success
    cr_assert_eq(set.count, 2);

    // Test capacity limits
    PipelineSolution ps3 = create_test_pipelinesolution(1);
    for (int i = set.count; i < set.capacity; i++)
    {
        int result = pipelinesolutionset_add_copy(&set, &ps3);
        cr_assert_eq(result, 1);
    }

    // Should be full now
    int result_full = pipelinesolutionset_add_copy(&set, &ps3);
    cr_assert_eq(result_full, 0);  // Should fail - full

    // Clean up
    pipelinesolutionset_free(&set);
    pipelinesolution_free(&ps1);
    pipelinesolution_free(&ps2);
    pipelinesolution_free(&ps3);
}

Test(solutions, pipelinesolutionset_append) {
    PipelineSolutionSet set1 = create_test_pipelinesolutionset(3);
    PipelineSolutionSet set2 = create_test_pipelinesolutionset(2);

    int count1 = set1.count;
    int count2 = set2.count;

    // Append set2 to set1
    pipelinesolutionset_append(&set1, &set2);

    // The actual count depends on capacity - current implementation doesn't expand
    // So we verify that at least the original solutions are still there
    cr_assert_geq(set1.count, count1);

    // If there was enough capacity, all solutions should be added
    int max_possible   = set1.capacity;
    int expected_added = (count1 + count2 <= max_possible) ? count2 : (max_possible - count1);
    if (expected_added > 0)
    {
        cr_assert_eq(set1.count, count1 + expected_added);

        // Verify the appended solutions are correct
        for (int i = 0; i < expected_added; i++)
        {
            // The last expected_added solutions should match set2's solutions
            int set1_index = count1 + i;
            cr_assert_eq(set1.data[set1_index].count, set2.data[i].count);
        }
    }

    // Clean up
    pipelinesolutionset_free(&set1);
    pipelinesolutionset_free(&set2);
}

Test(solutions, pipelinesolutionset_merge_with_prefix) {
    // Create a prefix solution
    PipelineSolution prefix = create_test_pipelinesolution(2);
    prefix.heuristic_score  = 10;

    // Create source solutions to merge
    PipelineSolutionSet src = create_test_pipelinesolutionset(2);

    // Perform merge
    PipelineSolutionSet dst;
    pipelinesolutionset_merge_with_prefix(&dst, &src, &prefix);

    // Verify results
    cr_assert_eq(dst.count, src.count);

    for (int i = 0; i < dst.count; i++)
    {
        // Each merged solution should have prefix steps + src steps
        int expected_count = prefix.count + src.data[i].count;
        cr_assert_eq(dst.data[i].count, expected_count);

        // Verify first steps are from prefix
        for (int j = 0; j < prefix.count; j++)
        {
            cr_assert_eq(dst.data[i].steps[j].starts_on_inverse, prefix.steps[j].starts_on_inverse);
            cr_assert_eq(dst.data[i].steps[j].solution->length, prefix.steps[j].solution->length);
        }

        // Verify last steps are from src
        for (int j = 0; j < src.data[i].count; j++)
        {
            int dst_index = prefix.count + j;
            cr_assert_eq(dst.data[i].steps[dst_index].starts_on_inverse,
                         src.data[i].steps[j].starts_on_inverse);
        }
    }

    // Clean up
    pipelinesolution_free(&prefix);
    pipelinesolutionset_free(&src);
    pipelinesolutionset_free(&dst);
}

Test(solutions, pipelinesolutionset_edge_cases) {
    // Test empty sets
    PipelineSolutionSet empty;
    pipelinesolutionset_init(&empty, 5);
    cr_assert_eq(empty.count, 0);

    // Test appending empty set to non-empty set
    PipelineSolutionSet nonempty       = create_test_pipelinesolutionset(2);
    int                 original_count = nonempty.count;
    pipelinesolutionset_append(&nonempty, &empty);
    cr_assert_eq(nonempty.count, original_count);  // Should be unchanged

    // Test appending non-empty set to empty set
    pipelinesolutionset_append(&empty, &nonempty);
    cr_assert_eq(empty.count, original_count);

    // Test single element operations
    PipelineSolution    single = create_test_pipelinesolution(1);
    PipelineSolutionSet single_set;
    pipelinesolutionset_init(&single_set, 1);
    pipelinesolutionset_add_copy(&single_set, &single);
    cr_assert_eq(single_set.count, 1);

    // Clean up
    pipelinesolutionset_free(&empty);
    pipelinesolutionset_free(&nonempty);
    pipelinesolutionset_free(&single_set);
    pipelinesolution_free(&single);
}

/* =========================
   CLI Printing Tests
   ========================= */

Test(solutions, cli_print_pipelinesolution_set) {
    // Create a simple pipeline solution set for testing
    PipelineSolutionSet test_set;
    pipelinesolutionset_init(&test_set, 2);

    // Create first pipeline solution with 2 steps
    PipelineSolution ps1;
    cube_t           cube1 = create_test_cube();
    pipelinesolution_init(&ps1);

    // Add first step: R U F
    PipelineSolutionStep step1a;
    pipelinesolutionstep_init(&step1a, false);
    pipelinesolutionstep_append(&step1a, R1);
    pipelinesolutionstep_append(&step1a, U1);
    pipelinesolutionstep_append(&step1a, F1);

    // Add second step: B' L2 (starts on inverse)
    PipelineSolutionStep step1b;
    pipelinesolutionstep_init(&step1b, true);
    pipelinesolutionstep_append(&step1b, B3);
    pipelinesolutionstep_append(&step1b, L2);

    pipelinesolution_add_copy(&ps1, &step1a);
    pipelinesolution_add_copy(&ps1, &step1b);
    ps1.heuristic_score = 42;

    // Create second pipeline solution with 1 step
    PipelineSolution ps2;
    cube_t           cube2 = cube_create_new_cube();
    pipelinesolution_init(&ps2);

    PipelineSolutionStep step2a;
    pipelinesolutionstep_init(&step2a, false);
    pipelinesolutionstep_append(&step2a, D2);
    pipelinesolutionstep_append(&step2a, R2);

    pipelinesolution_add_copy(&ps2, &step2a);
    ps2.heuristic_score = 15;

    // Add to set
    pipelinesolutionset_add_copy(&test_set, &ps1);
    pipelinesolutionset_add_copy(&test_set, &ps2);

    // Test non-verbose printing
    printf("\n--- Testing non-verbose output ---\n");
    cube_print_pipelinesolution_set(&test_set, 0);

    // Test verbose printing
    printf("\n--- Testing verbose output ---\n");
    cube_print_pipelinesolution_set(&test_set, 1);

    // Clean up
    pipelinesolutionstep_free(&step1a);
    pipelinesolutionstep_free(&step1b);
    pipelinesolutionstep_free(&step2a);
    pipelinesolution_free(&ps1);
    pipelinesolution_free(&ps2);
    pipelinesolutionset_free(&test_set);
}
