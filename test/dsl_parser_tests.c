#include "criterion/criterion.h"

#include "../src/dsl.h"

/* ---------------------------------------------------------------- */
/* helpers                                                          */
/* ---------------------------------------------------------------- */

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


/* ---------------------------------------------------------------- */
/* parser: structure, precedence, piece/group expansion             */
/* ---------------------------------------------------------------- */
Test(dsl_parse, precedence_and_over_or) {
    canon_is("eoud | eolr & eofb", "(eoud | (eolr & eofb))");
}

Test(dsl_parse, parens_override_precedence) {
    canon_is("(eoud | eolr) & eofb", "((eoud | eolr) & eofb)");
}

Test(dsl_parse, not_binds_tightest) {
    canon_is("!eofb:UF", "!(eofb:UF)");
    canon_is("!!eofb", "!(!(eofb))");
    canon_is("!(eofb & eoud)", "!((eofb & eoud))");
}

Test(dsl_parse, missing_pieces_means_all) {
    canon_is("eofb", "eofb");
    canon_is("solved", "solved");
}

Test(dsl_parse, star_wildcard_removed) {
    parse_errs("eofb:*");
    parse_errs("solved:*,UF");
}

Test(dsl_parse, piece_list_rendered_in_bit_order) {
    canon_is("solved:UFL,UFR", "solved:UFR,UFL");
    canon_is("eofb:UF,UB", "eofb:UB,UF");
}

Test(dsl_parse, groups_expand_to_pieces) {
    canon_is("eoud:Ue", "eoud:UB,UR,UF,UL");
    canon_is("solved:U", "solved:UB,UR,UF,UL,UBL,UBR,UFR,UFL");
    canon_is("solved:Uc", "solved:UBL,UBR,UFR,UFL");
    canon_is("solved:Dw", "solved:DF,DR,DB,DL,BL,BR,FR,FL,DFL,DFR,DBR,DBL");
    canon_is("eofb:M", "eofb:UB,UF,DF,DB");
    canon_is("coud:D", "coud:DF,DR,DB,DL,DFL,DFR,DBR,DBL");
}

Test(dsl_parse, whitespace_tolerated) {
    canon_is(" eofb : UF ", "eofb:UF");
    canon_is("eoud |  eolr\t&  eofb", "(eoud | (eolr & eofb))");
}

Test(dsl_parse, associativity) {
    canon_is("eofb & eolr & eoud", "((eofb & eolr) & eoud)");
    canon_is("eofb | eolr | eoud", "((eofb | eolr) | eoud)");
}

Test(dsl_parse, errors_return_null_with_message) {
    parse_errs("");
    parse_errs("& eofb");
    parse_errs("eofb |");
    parse_errs("eofb:UF )");
    parse_errs("eofb:XX");
    parse_errs("foo:UF");
    parse_errs("(eofb");
    parse_errs("eofb:");
    parse_errs("eofb:UF,");
}

/* ---------------------------------------------------------------- */
/* canonical round-trip                                             */
/* ---------------------------------------------------------------- */

Test(dsl_parser, canonical_is_idempotent) {
    const char* exprs[] = {
        "eofb",
        "eoud:Ue",
        "solved:Dw",
        "(eofb | eolr) & eoud",
        "!(eofb & eolr)",
        "solved:UFR,UFL & eofb:UF,UB",
        "eofb:UF,UB,UR,UL",
        "eoud | eolr & eofb",
        "!solved",
        "ep:Ue",
        "cp:U",
        "coud:Uc | colr:Dc",
        "eofb:M & eoud:E",
    };
    size_t n = sizeof(exprs) / sizeof(exprs[0]);
    for (size_t i = 0; i < n; i++) {
        dsl_expr_t* e = parse_ok(exprs[i]);
        char* c1 = dsl_canonical(e);
        dsl_expr_t* e2 = parse_ok(c1);
        char* c2 = dsl_canonical(e2);
        cr_assert_str_eq(c1, c2, "roundtrip failed for \"%s\"", exprs[i]);
        free(c1);
        free(c2);
        dsl_free_expression(e);
        dsl_free_expression(e2);
    }
}
