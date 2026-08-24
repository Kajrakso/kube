#include "criterion/criterion.h"

#include "../src/dsl.h"
#include "../src/core/move.h"

/* ---------------------------------------------------------------- */
/* helpers                                                          */
/* ---------------------------------------------------------------- */

static dsl_expr_t* parse_ok(const char* s) {
    char err[256];
    dsl_expr_t* e = dsl_parse(s, err, sizeof err);
    cr_assert_not_null(e, "parse failed for \"%s\": %s", s, err);
    return e;
}

static cube_t cube_after(const char* moves) {
    cube_t cube = cube_create_new_cube();
    cube_move_apply_move_string(&cube, moves);
    return cube;
}

static bool eval_on(const char* expr, cube_t* cube) {
    dsl_expr_t* e = parse_ok(expr);
    bool r = dsl_eval(e, cube);
    dsl_free_expression(e);
    return r;
}

/* ---------------------------------------------------------------- */
/* evaluation                                                       */
/* ---------------------------------------------------------------- */

Test(dsl_eval, solved_cube_satisfies_all_prims) {
    cube_t cube = cube_create_new_cube();
    const char* prims[] = {"eofb", "eolr", "eoud", "cofb", "colr", "coud",
                           "ep", "cp", "solved"};
    size_t n = sizeof(prims) / sizeof(prims[0]);
    for (size_t i = 0; i < n; i++) {
        char expr[32];
        snprintf(expr, sizeof expr, "%s", prims[i]);
        cr_assert(eval_on(expr, &cube), "\"%s\" should hold on the solved cube", expr);
    }
    cr_assert_not(eval_on("!eofb", &cube));
    cr_assert(eval_on("eofb & eolr", &cube));
    cr_assert(eval_on("eofb | !eolr", &cube));
}

Test(dsl_eval, move_spot_checks) {
    cube_t cube = cube_after("R L");
    cr_assert(eval_on("eofb", &cube));
    cr_assert(eval_on("eoud", &cube));
    cr_assert_not(eval_on("coud", &cube));

    cube = cube_after("U D");
    cr_assert(eval_on("eofb", &cube));
    cr_assert_not(eval_on("eoud", &cube));
    cr_assert(eval_on("coud", &cube));

    cube = cube_after("F B");
    cr_assert_not(eval_on("eofb", &cube));
    cr_assert(eval_on("eoud", &cube));
}

Test(dsl_eval, layer_and_group_masks) {
    cube_t cube = cube_after("U");
    cr_assert(eval_on("solved:Dw", &cube), "D-layer pieces are untouched by U");
    cr_assert(eval_on("solved:Dc", &cube));
    cr_assert(eval_on("cp:D", &cube));
    cr_assert_not(eval_on("solved:Uc", &cube));
    cr_assert_not(eval_on("solved", &cube));

    cube = cube_after("R");
    cr_assert_not(eval_on("solved:Dw", &cube), "R touches D-layer pieces");
}

Test(dsl_eval, is_fin) {
    dsl_expr_t* e = parse_ok("solved");
    cr_assert(dsl_is_maybe_fin(e));
    dsl_free_expression(e);
    e = parse_ok("solved");
    cr_assert(dsl_is_maybe_fin(e));
    dsl_free_expression(e);
    e = parse_ok("eofb");
    cr_assert_not(dsl_is_maybe_fin(e));
    dsl_free_expression(e);
    e = parse_ok("solved:UF");
    cr_assert_not(dsl_is_maybe_fin(e));
    dsl_free_expression(e);
    e = parse_ok("solved:Uc & solved:Dc");
    cr_assert_not(dsl_is_maybe_fin(e));
    dsl_free_expression(e);
}

