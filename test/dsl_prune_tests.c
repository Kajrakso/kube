#include <stdlib.h>
#include <string.h>

#include "criterion/criterion.h"

#include "../src/dsl.h"
#include "../src/dsl_prune.h"
#include "../src/moveset.h"
#include "../src/core/move.h"
#include "../src/core/cube_operation.h"
#include "../src/tables.h"

static cube_t cube_after(const char* moves) {
    cube_t cube = cube_create_new_cube();
    cube_move_apply_move_string(&cube, moves);
    return cube;
}

static void gen_and_load_custom(ptable_data_t** tabs, uint8_t n) {
    dsl_prune_step_ctx_t* sc =
        (dsl_prune_step_ctx_t*)tabs[0]->custom_data;
    cr_assert_not_null(sc);

    for (uint8_t k = 0; k < n; k++) {
        ptable_data_t* t = tabs[k];
        custom_prune_table_ctx_t* tc =
            (k == 0) ? &sc->table_ctxs[0]
                     : (custom_prune_table_ctx_t*)t->custom_data;

        void* saved = NULL;
        if (k == 0) {
            saved = t->custom_data;
            t->custom_data = tc;
        }
        t->moveset_mask = MOVESET_HTM;

        bool edges = tc->kind == PRUNE_EDGES;
        ptable_gen_ctx_t g;
        memset(&g, 0, sizeof g);
        g.ptable_data = t;
        g.setup = NULL;
        g.init = edges ? dsl_prune_init_edges : dsl_prune_init_corners;
        g.apply_move = NULL;
        g.decompose_index =
            edges ? dsl_prune_decompose_edges : dsl_prune_decompose_corners;
        g.dls_max_depth = 8;
        g.nbhr_min_depth = 8;
        g.nbhr_max_depth_excl = 8;
        g.num_components = 3;

        table_prune_gen(&g);

        if (k == 0) t->custom_data = saved;

        cr_assert_eq(cube_tables_load_ptable(t), 0,
                     "loading generated table \"%s\"", t->name);
        cr_assert(t->ptable_is_loaded);
        cr_assert_not_null(t->ptable);
    }
}

static size_t run_heuristic(ptable_data_t** tabs, cube_t* c,
                            size_t (*fn)(cube_t*, solving_step*)) {
    solving_step ss;
    memset(&ss, 0, sizeof ss);
    ss.p_data = tabs[0];
    return fn(c, &ss);
}

Test(dsl_ptable, make_ptables_groups_solved_atoms) {
    char err[256];
    dsl_expr_t* e = dsl_parse("solved:*", err, sizeof err);
    cr_assert_not_null(e);

    uint8_t n = 0;
    ptable_data_t** tabs = dsl_prune_make_ptables(e, &n);
    cr_assert_eq(n, 6);
    cr_assert_not_null(tabs);

    uint16_t em_union = 0;
    uint8_t cm_union = 0;
    for (int i = 0; i < n; i++) {
        custom_prune_table_ctx_t* tc =
            (i == 0)
                ? &((dsl_prune_step_ctx_t*)tabs[0]->custom_data)->table_ctxs[0]
                : (custom_prune_table_ctx_t*)tabs[i]->custom_data;
        cr_assert_eq(tc->kind, i < 4 ? PRUNE_EDGES : PRUNE_CORNERS, "table %d", i);
        cr_assert_eq(tc->n, i < 4 ? 3 : 4, "table %d", i);
        for (int j = 1; j < tc->n; j++)
            cr_assert_lt(tc->pieces[j - 1], tc->pieces[j]);
        em_union |= tc->edge_mask;
        cm_union |= tc->corner_mask;
        if (i < 4)
            cr_assert_eq(tabs[i]->number_of_elements, 220ULL * 8ULL * 6ULL);
        else
            cr_assert_eq(tabs[i]->number_of_elements, 70ULL * 81ULL * 24ULL);
    }
    cr_assert_eq(em_union, 0x0FFF);
    cr_assert_eq(cm_union, 0xFF);

    dsl_prune_free_tables(tabs, n);
    dsl_free_expression(e);
}

Test(dsl_ptable, no_solved_atoms_no_tables) {
    char err[256];
    const char* exprs[] = {"eofb:*", "!(solved:UF)", "eofb | coud"};
    for (size_t i = 0; i < sizeof exprs / sizeof exprs[0]; i++) {
        dsl_expr_t* e = dsl_parse(exprs[i], err, sizeof err);
        cr_assert_not_null(e);
        uint8_t n = 7;
        ptable_data_t** tabs = dsl_prune_make_ptables(e, &n);
        cr_assert_null(tabs, "\"%s\"", exprs[i]);
        cr_assert_eq(n, 0, "\"%s\"", exprs[i]);
        dsl_free_expression(e);
    }
}

Test(dsl_ptable, single_edge_heuristic_values) {
    char err[256];
    dsl_expr_t* e = dsl_parse("solved:UF", err, sizeof err);
    cr_assert_not_null(e);

    uint8_t n = 0;
    ptable_data_t** tabs = dsl_prune_make_ptables(e, &n);
    cr_assert_eq(n, 1);
    gen_and_load_custom(tabs, n);

    const struct { const char* moves; size_t h; } cases[] = {
        {"", 0}, {"R", 0}, {"R L", 0},
        {"U", 1}, {"U2", 1}, {"U'", 1}, {"F", 1}, {"R U", 1},
    };
    for (size_t i = 0; i < sizeof cases / sizeof cases[0]; i++) {
        cube_t c = cube_after(cases[i].moves);
        size_t h = run_heuristic(tabs, &c, dsl_prune_heuristic);
        cr_assert_eq(h, cases[i].h, "scramble \"%s\"", cases[i].moves);
        cr_assert_eq(h == 0, dsl_eval(e, &c),
                     "h/eval mismatch on \"%s\"", cases[i].moves);
    }

    dsl_prune_free_tables(tabs, n);
    dsl_free_expression(e);
}

Test(dsl_ptable, heuristic_lipschitz_over_moves) {
    char err[256];
    dsl_expr_t* e = dsl_parse("solved:Dw & coud:U", err, sizeof err);
    cr_assert_not_null(e);

    uint8_t n = 0;
    ptable_data_t** tabs = dsl_prune_make_ptables(e, &n);
    cr_assert(n >= 2);
    gen_and_load_custom(tabs, n);

    const char* scrambles[] = {
        "", "U", "R", "F R U R' U' F'", "L' U' L U' L' U2 L",
        "R U R' U' R' F R2 U' R' U' R U R' F'",
    };
    for (size_t i = 0; i < sizeof scrambles / sizeof scrambles[0]; i++) {
        cube_t c = cube_after(scrambles[i]);
        size_t h0 = run_heuristic(tabs, &c, dsl_prune_heuristic);
        for (int m = 0; m < NMOVES; m++) {
            cube_t c2 = c;
            cube_move_apply_move(&c2, m);
            size_t h1 = run_heuristic(tabs, &c2, dsl_prune_heuristic);
            size_t d = h1 > h0 ? h1 - h0 : h0 - h1;
            cr_assert_leq(d, (size_t)1,
                         "|dh|>1 after %s on \"%s\"",
                         move_notation[m], scrambles[i]);
        }
    }

    dsl_prune_free_tables(tabs, n);
    dsl_free_expression(e);
}

