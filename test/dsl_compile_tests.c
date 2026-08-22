#include <stdlib.h>
#include <string.h>

#include "criterion/criterion.h"

#include "../src/dsl_compile.h"
#include "../src/dsl_prune.h"
#include "../src/moveset.h"
#include "../src/solver.h"

static struct arguments args_one(const char* name, const char* expr,
                                 uint32_t moveset_mask) {
    struct arguments a;
    set_default_values_arguments(&a);
    a.def_count = 1;
    a.defs[0].name = strdup(name);
    a.defs[0].expr = strdup(expr);
    a.defs[0].moveset_mask = moveset_mask;
    return a;
}

static void args_free(struct arguments* a) {
    for (int i = 0; i < a->def_count; i++) {
        free(a->defs[i].name);
        free(a->defs[i].expr);
    }
}

Test(dsl_compile, solved_star_routes_to_optimal_solver) {
    struct arguments a = args_one("x", "solved:*", MOVESET_HTM);
    dsl_compile_definition_t* defs = NULL;
    int n = 0;

    cr_assert_eq(dsl_compile_build_definitions(&defs, &n, &a), 0);
    cr_assert_eq(n, 1);
    cr_assert_str_eq(defs[0].name, "x");
    cr_assert_str_eq(defs[0].canonical, "solved:*");
    cr_assert_not_null(defs[0].expr);

    cr_assert_not(defs[0].step.is_custom);
    cr_assert_null(defs[0].step.custom_ptables);
    cr_assert_eq(defs[0].step.p_data, fin.p_data);
    cr_assert_eq(defs[0].step.heuristic_func, fin.heuristic_func);
    cr_assert_eq(defs[0].step.moveset_mask, MOVESET_HTM);
    cr_assert_str_eq(defs[0].step.name, "x");

    dsl_compile_free_definitions(defs, &n);
    cr_assert_eq(n, 0);
    args_free(&a);
}

Test(dsl_compile, set_atom_compiles_to_custom_step) {
    struct arguments a = args_one("x", "{U}", MOVESET_HTM);
    dsl_compile_definition_t* defs = NULL;
    int n = 0;

    cr_assert_eq(dsl_compile_build_definitions(&defs, &n, &a), 0);
    cr_assert(defs[0].step.is_custom);
    cr_assert_not_null(defs[0].step.cube_is_solved);
    /* set atoms have no solved: pieces, so no pruning tables are built */
    cr_assert_null(defs[0].step.custom_ptables);
    cr_assert_eq(defs[0].step.n_custom_ptables, 0);
    cr_assert_str_eq(defs[0].canonical, "{U}");

    cube_t solved = cube_create_new_cube();
    /* {U} is the singleton set: the solved cube is NOT a member */
    cr_assert_not(
        defs[0].step.cube_is_solved(&solved, defs[0].step.custom_data));
    cube_t u = solved;
    cube_move_apply_move_string(&u, "U");
    cr_assert(defs[0].step.cube_is_solved(&u, defs[0].step.custom_data));
    cube_t r = solved;
    cube_move_apply_move_string(&r, "R");
    cr_assert_not(
        defs[0].step.cube_is_solved(&r, defs[0].step.custom_data));

    dsl_compile_free_definitions(defs, &n);
    args_free(&a);
}

Test(dsl_compile, general_expr_gets_custom_tables_and_heuristic) {
    struct arguments a = args_one("f2l", "solved:Dw&coud:U", MOVESET_HTM);
    dsl_compile_definition_t* defs = NULL;
    int n = 0;

    cr_assert_eq(dsl_compile_build_definitions(&defs, &n, &a), 0);
    cr_assert(defs[0].step.is_custom);
    cr_assert_not_null(defs[0].step.custom_ptables);
    cr_assert_gt(defs[0].step.n_custom_ptables, 0);
    cr_assert_eq(defs[0].step.heuristic_func, dsl_prune_heuristic);
    cr_assert_not_null(defs[0].step.cube_is_solved);
    cr_assert_eq(defs[0].step.p_data, defs[0].step.custom_ptables[0]);
    cr_assert_str_eq(
        defs[0].canonical,
        "(solved:DF,DR,DB,DL,BL,BR,FR,FL,DFL,DFR,DBR,DBL & coud:UB,UR,UF,UL,UBL,UBR,UFR,UFL)");

    cube_t solved = cube_create_new_cube();
    cr_assert(defs[0].step.cube_is_solved(&solved, defs[0].step.custom_data));
    cube_t r = solved;
    cube_move_apply_move_string(&r, "R");
    cr_assert_not(
        defs[0].step.cube_is_solved(&r, defs[0].step.custom_data));

    dsl_compile_free_definitions(defs, &n);
    args_free(&a);
}

Test(dsl_compile, invalid_expr_fails_but_is_freeable) {
    struct arguments a = args_one("x", "foo:UF", MOVESET_HTM);
    dsl_compile_definition_t* defs = NULL;
    int n = 0;

    cr_assert_eq(dsl_compile_build_definitions(&defs, &n, &a), 1);
    dsl_compile_free_definitions(defs, &n);
    cr_assert_eq(n, 0);
    args_free(&a);
}

Test(dsl_compile, custom_moveset_survives_compilation) {
    uint32_t mask;
    char err[64];
    cr_assert_eq(moveset_parse("QTM", &mask, err, sizeof err), 0);

    struct arguments a = args_one("q", "solved:*", mask);
    dsl_compile_definition_t* defs = NULL;
    int n = 0;

    cr_assert_eq(dsl_compile_build_definitions(&defs, &n, &a), 0);
    cr_assert(defs[0].step.is_custom);
    cr_assert_eq(defs[0].step.moveset_mask, mask);
    cr_assert_eq(defs[0].step.p_data, &ptable_data_opt1);
    cr_assert_not_null(defs[0].step.heuristic_func);

    dsl_compile_free_definitions(defs, &n);
    args_free(&a);
}

Test(dsl_compile, integration_dsl_definition_solves_optimally) {
    struct arguments a = args_one("x", "solved:*", MOVESET_HTM);
    dsl_compile_definition_t* defs = NULL;
    int n = 0;

    cr_assert_eq(dsl_compile_build_definitions(&defs, &n, &a), 0);
    cr_assert_not(defs[0].step.is_custom);

    cube_t c = cube_create_new_cube();
    cube_move_apply_move_string(&c, "R");

    SolutionSet ss;
    solutionset_init(&ss, 1);
    cube_solvers_solve_cube(c, &ss, 1, 20, 0, 16, &defs[0].step);

    cr_assert_eq(ss.count, 1);
    cr_assert_eq(ss.data[0].length, 1);
    cr_assert_eq(ss.data->moves[0], R3);

    solutionset_free(&ss);
    dsl_compile_free_definitions(defs, &n);
    args_free(&a);
}
