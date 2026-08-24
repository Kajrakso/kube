#include <stdlib.h>

#include "criterion/criterion.h"

#include "../src/dsl.h"
#include "../src/core/move.h"
#include "../src/core/cube_operation.h"


static dsl_expr_t* parse_ok(const char* s) {
    char err[256];
    dsl_expr_t* e = dsl_parse(s, err, sizeof err);
    cr_assert_not_null(e, "parse failed for \"%s\": %s", s, err);
    return e;
}

static void canon_is(const char* input, const char* expected) {
    dsl_expr_t* e = parse_ok(input);
    char* c = dsl_canonical(e);
    cr_assert_str_eq(c, expected, "canonical of \"%s\"", input);
    free(c);
    dsl_free_expression(e);
}

static void parse_errs(const char* s) {
    char err[256];
    dsl_expr_t* e = dsl_parse(s, err, sizeof err);
    cr_assert_null(e, "expected parse failure for \"%s\"", s);
    cr_assert(err[0] != '\0', "expected non-empty error message for \"%s\"", s);
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

Test(dsl_set, explicit_set_parses_and_renders) {
    dsl_expr_t* e = parse_ok("{R, U2}");
    cr_assert_eq(e->kind, EXPR_ATOM);
    cr_assert_eq(e->atom_kind, ATOM_SET);
    /* exactly the listed elements: no implicit identity */
    cr_assert_eq(e->subgroup_len, 2);
    cube_t r = cube_after("R");
    cr_assert(cube_operation_is_equal(e->subgroup_elems[0], r));
    char* c = dsl_canonical(e);
    cr_assert_str_eq(c, "{R,U2}");
    free(c);
    dsl_free_expression(e);
}

Test(dsl_set, identity_tokens_e_and_I) {
    dsl_expr_t* e = parse_ok("{e}");
    cr_assert_eq(e->atom_kind, ATOM_SET);
    cr_assert_eq(e->subgroup_len, 1);
    dsl_free_expression(e);

    e = parse_ok("{I}");
    cr_assert_eq(e->subgroup_len, 1);
    dsl_free_expression(e);

    /* listing the identity explicitly does not duplicate it */
    e = parse_ok("{U,e}");
    cr_assert_eq(e->subgroup_len, 2);
    dsl_free_expression(e);

    /* duplicates are removed */
    e = parse_ok("{U,U,U2}");
    cr_assert_eq(e->subgroup_len, 2);
    dsl_free_expression(e);

    cube_t solved = cube_create_new_cube();
    cr_assert(eval_on("{e}", &solved));
    cube_t r = cube_after("R");
    cr_assert_not(eval_on("{e}", &r));
}

Test(dsl_set, generator_closure_sizes) {
    const struct { const char* spec; int len; } cases[] = {
        {"<U>", 4},
        {"<D>", 4},
        {"<R>", 4},
        {"<U,D>", 16},
        {"<L,R>", 16},
        /* moves inside one generator are composed: "U U" == U2 */
        {"<U U>", 2},
        {"<U,U>", 4},
    };
    for (size_t i = 0; i < sizeof cases / sizeof cases[0]; i++) {
        dsl_expr_t* e = parse_ok(cases[i].spec);
        cr_assert_eq(e->kind, EXPR_ATOM, "%s", cases[i].spec);
        cr_assert_eq(e->atom_kind, ATOM_SET, "%s", cases[i].spec);
        cr_assert_eq(e->set_is_generators, true, "%s", cases[i].spec);
        cr_assert_eq(e->subgroup_len, cases[i].len,
                     "%s: expected %d elements", cases[i].spec, cases[i].len);
        dsl_free_expression(e);
    }
}

Test(dsl_set, eval_membership) {
    cube_t solved = cube_create_new_cube();
    cr_assert_not(eval_on("{U}", &solved));
    cr_assert_not(eval_on("{U,U2,U'}", &solved));
    cr_assert(eval_on("{e}", &solved));

    cube_t u = cube_after("U");
    cr_assert(eval_on("{U}", &u));
    cr_assert_not(eval_on("{U2,U'}", &u));

    cube_t r = cube_after("R");
    cr_assert_not(eval_on("{U,U2,U'}", &r));

    /* generator closure: <R> contains R, R2, R' */
    cr_assert(eval_on("<R>", &r));
    cube_t r2 = cube_after("R2");
    cr_assert(eval_on("<R>", &r2));
    cube_t f = cube_after("F");
    cr_assert_not(eval_on("<R>", &f));
}

Test(dsl_set, boolean_combinations) {
    cube_t u = cube_after("U");
    cr_assert(eval_on("eofb & {U}", &u));
    cr_assert(eval_on("!{R} & {U}", &u));
    cr_assert(eval_on("{R} | {U}", &u));
    cr_assert_not(eval_on("!{U} & !eofb", &u));
}

Test(dsl_set, explicit_elements_can_be_sequences) {
    dsl_expr_t* e = parse_ok("{e, R U R'}");
    cr_assert_eq(e->kind, EXPR_ATOM);
    cr_assert_eq(e->atom_kind, ATOM_SET);
    cr_assert_eq(e->subgroup_len, 2);

    cube_t solved = cube_create_new_cube();
    cr_assert(eval_on("{e, R U R'}", &solved));

    /* "R U R'" denotes one state: the composition of the three moves */
    cube_t rur = cube_after("R U R'");
    cr_assert(eval_on("{e, R U R'}", &rur));
    cube_t rurf = cube_after("R U R' F");
    cr_assert_not(eval_on("{e, R U R'}", &rurf));

    char* c = dsl_canonical(e);
    cr_assert_str_eq(c, "{e,R U R'}");
    free(c);
    dsl_free_expression(e);
}

Test(dsl_set, sequence_element_state_equality) {
    /* R U2 R' and R U R' are different states */
    cube_t a = cube_after("R U2 R'");
    cr_assert_not(eval_on("{R U R'}", &a));
    cube_t b = cube_after("R U R'");
    cr_assert(eval_on("{R U R'}", &b));

    /* spelled-out identity inside a sequence: "U e U'" == e */
    cube_t solved = cube_create_new_cube();
    cr_assert(eval_on("{U e U'}", &solved));
    cube_t u = cube_after("U");
    cr_assert_not(eval_on("{U e U'}", &u));
    /* redundant identity mid-sequence: "R e" == R */
    cube_t r = cube_after("R");
    cr_assert(eval_on("{R e}", &r));
}

Test(dsl_set, generator_closure_eval_matches_explicit) {
    const char* scrambles[] = {"", "U", "R", "U R'", "F2 U L'",
                               "R U R' U'", "L2 D B'"};
    for (size_t i = 0; i < sizeof scrambles / sizeof scrambles[0]; i++) {
        cube_t c1 = cube_after(scrambles[i]);
        cube_t c2 = c1;
        cr_assert_eq(eval_on("<R>", &c1),
                     eval_on("{R,R',R2,e}", &c2),
                     "scramble \"%s\"", scrambles[i]);
    }
}

/* ---------------------------------------------------------------- */
/* product: A*B = {a*b : a in A, b in B} (apply a first, then b)     */
/* ---------------------------------------------------------------- */

Test(dsl_prod, parse_precedence_and_canonical) {
    canon_is("solved * {R}", "(solved * {R})");
    canon_is("{U} * <R>", "({U} * <R>)");
    /* binds tighter than & and |, looser than unary ! */
    canon_is("eofb & {U} * <R>", "(eofb & ({U} * <R>))");
    canon_is("!{U} * {R}", "(!({U}) * {R})");
    /* left-associative chain (the product is associative) */
    canon_is("{U} * {R} * {F}", "(({U} * {R}) * {F})");
}

Test(dsl_prod, requires_a_set_operand) {
    parse_errs("eofb * coud");
    parse_errs("(eofb & solved:UF) * solved:D");
}

Test(dsl_prod, both_sets_materialized) {
    cube_t r = cube_after("R");
    cr_assert(eval_on("{e} * {R}", &r));
    cr_assert(eval_on("{R} * {e}", &r));

    cube_t ru = cube_after("R U");
    cr_assert(eval_on("{R} * {U}", &ru));
    cr_assert_not(eval_on("{U} * {R}", &ru));   /* order matters */

    /* duplicates collapse through the materialized index */
    cube_t ur = cube_after("U R");
    cr_assert(eval_on("{U,U2} * {R,R'}", &ur));
}

Test(dsl_prod, mixed_set_predicate_membership) {
    /* predicate * set: c = p*h  <=>  c*h^-1 satisfies the predicate */
    cube_t r = cube_after("R");
    cr_assert(eval_on("solved:UF * {R}", &r));
    cube_t f = cube_after("F");
    cr_assert_not(eval_on("solved:UF * {R}", &f));

    /* set * predicate: c = h*p  <=>  h^-1*c satisfies the predicate.
     * U preserves edge orientation, so it cannot fix the flips from F. */
    cube_t u = cube_after("U");
    cr_assert(eval_on("<U> * eofb", &u));
    cube_t fr = cube_after("F R");
    cr_assert_not(eval_on("<U> * eofb", &fr));
}

Test(dsl_prod, matches_manual_transform_loop) {
    dsl_expr_t* e = parse_ok("{R,U'} * solved:UF");
    dsl_expr_t* pred = parse_ok("solved:UF");
    cube_t elems[2] = {cube_after("R"), cube_after("U'")};
    const char* scrambles[] = {"", "R", "U'", "R' U", "F", "U R'",
                               "R U R' U'", "L2 D B'"};
    for (size_t i = 0; i < sizeof scrambles / sizeof scrambles[0]; i++) {
        cube_t c = cube_after(scrambles[i]);
        bool want = false;
        for (int j = 0; j < 2 && !want; j++) {
            cube_t inv = cube_operation_inverse(elems[j]);
            /* c = a*p  <=>  a^-1*c satisfies the predicate */
            cube_t transformed = cube_operation_compose(inv, c);
            if (dsl_eval(pred, &transformed)) want = true;
        }
        cr_assert_eq(dsl_eval(e, &c), want, "scramble \"%s\"", scrambles[i]);
    }
    dsl_free_expression(e);
    dsl_free_expression(pred);
}

Test(dsl_set, canonical_rendering) {
    canon_is("{R, U2}", "{R,U2}");
    /* elements render in input order */
    canon_is("{U2,R,e}", "{U2,R,e}");
    canon_is("<U,D>", "<U,D>");
    canon_is("<U U,F2>", "<U U,F2>");
    /* duplicate generators are removed */
    canon_is("<U,U>", "<U>");
    /* sets bind tightest (primary level), like primitives */
    canon_is("eofb & {U}", "(eofb & {U})");
    canon_is("!{R} | eofb", "(!({R}) | eofb)");
    canon_is("(eofb | {U}) & {R}", "((eofb | {U}) & {R})");
}

Test(dsl_set, parse_errors) {
    parse_errs("{}");
    parse_errs("<>");
    parse_errs("{X}");
    parse_errs("{R");
    parse_errs("<R");
    parse_errs("{R,}");
    parse_errs("{R} extra");
}

Test(dsl_set, nested_expression_free) {
    dsl_expr_t* e = parse_ok("!(eofb | <U,D>)");
    cr_assert_eq(e->kind, EXPR_NOT);
    dsl_expr_t* set = e->left->right;
    cr_assert_eq(set->kind, EXPR_ATOM);
    cr_assert_eq(set->atom_kind, ATOM_SET);
    cr_assert_eq(set->subgroup_len, 16);
    dsl_free_expression(e);

    e = parse_ok("!({R,U})");
    cr_assert_eq(e->kind, EXPR_NOT);
    cr_assert_eq(e->left->kind, EXPR_ATOM);
    cr_assert_eq(e->left->atom_kind, ATOM_SET);
    dsl_free_expression(e);
}
