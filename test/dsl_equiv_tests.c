#include "criterion/criterion.h"

#include "../src/dsl.h"
#include "../src/core/move.h"
#include "../src/core/cube_state.h"

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
/* equivalence against cube_state over scrambles                    */
/* ---------------------------------------------------------------- */

static void check_equiv(const char* expr, bool (*pred)(cube_t*)) {
    const char* scrambles[] = {
        "R", "U", "F", "R2", "U2", "R L", "U D", "F B",
        "R U R' U'",
        "F R U R' U' F'",
        "R U R' U R U2 R'",
        "L' U' L U' L' U2 L",
        "R U R' U' R' F R2 U' R' U' R U R' F'",
        "R' U' F' U F R",
        "F2 U L R' F2 L' R U F2",
        "R' F2 R U2 R B2 L' U2 R'",
        "U' R2 B2 D2 R B' D2 F D' L2 F2 L2 D2 L2 B2 U2 F' B2 R2 B'",
    };
    size_t n = sizeof(scrambles) / sizeof(scrambles[0]);
    for (size_t i = 0; i < n; i++) {
        cube_t cube = cube_after(scrambles[i]);
        bool a = eval_on(expr, &cube);
        bool b = pred(&cube);
        cr_assert_eq(a, b, "mismatch for \"%s\" on scramble \"%s\"", expr, scrambles[i]);
    }
}

Test(dsl_equiv, eofb_matches_is_eo_FB) {
    check_equiv("eofb", cube_state_is_eo_FB);
}

Test(dsl_equiv, eoud_matches_is_eo_UD) {
    check_equiv("eoud", cube_state_is_eo_UD);
}

Test(dsl_equiv, cofb_matches_is_co_FB) {
    check_equiv("cofb", cube_state_is_co_FB);
}

Test(dsl_equiv, any_eo_matches_is_eo) {
    check_equiv("eofb | eolr | eoud", cube_state_is_eo);
}

Test(dsl_equiv, dr_fb_formula) {
    check_equiv("eolr & eoud & cofb", cube_state_is_dr_FB);
}

Test(dsl_equiv, solved_matches_is_solved) {
    check_equiv("solved", cube_state_is_solved);
}

