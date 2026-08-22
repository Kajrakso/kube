#include "dsl_prune.h"

/* AI generated */


#include "index.h"
#include "utils/utils.h"
#include "utils/sha1.h"

#include <math.h>
#include <string.h>
#include <stdlib.h>

/* ------------------------------------------------------------------ */
/* helpers                                                             */
/* ------------------------------------------------------------------ */

/* Count set bits in a mask and write their positions to bits[].
 * Returns the number of set bits. */
static int extract_set_bits_u16(uint16_t mask, int* bits) {
    int n = 0;
    for (int i = 0; i < 16; i++)
        if (mask & (uint16_t)(1u << i))
            bits[n++] = i;
    return n;
}

static int extract_set_bits_u8(uint8_t mask, int* bits) {
    int n = 0;
    for (int i = 0; i < 8; i++)
        if (mask & (uint8_t)(1u << i))
            bits[n++] = i;
    return n;
}

/* ------------------------------------------------------------------ */
/* create individual tables                                            */
/* ------------------------------------------------------------------ */

ptable_data_t* dsl_prune_make_edge_table(int* edges, int n) {
    if (n < 1 || n > 4) return NULL;

    custom_prune_table_ctx_t* ctx = calloc(1, sizeof(*ctx));
    ctx->kind = PRUNE_EDGES;
    ctx->n = n;
    for (int i = 0; i < n; i++) {
        ctx->pieces[i] = edges[i];
        ctx->edge_mask |= (uint16_t)(1u << edges[i]);
    }
    ctx->corner_mask = 0;

    ptable_data_t* p = calloc(1, sizeof(*p));
    p->number_of_elements = (uint64_t)comb(NEDGES, n)
                          * (1ULL << (unsigned)n)
                          * factorial(n);
    p->ptable_size = (p->number_of_elements + 1) / 2;
    p->cube_to_index_func = NULL; // this should be partial_e_index somehow
    p->read_value_ptable_func = ptable_read_val_2_values_per_byte;
    p->set_value_ptable_func = ptable_set_val_2_values_per_byte;
    p->is_custom = true;
    p->custom_data = ctx;
    p->ptable_is_loaded = false;
    p->ptable = NULL;
    p->moveset_mask = 0;
    p->gen_ptable_func = NULL;

    /* generate a filename from the piece list */
    char key[128] = {0};
    int pos = 0;
    for (int i = 0; i < n; i++)
        pos += snprintf(key + pos, sizeof(key) - (size_t)pos, "%d,", edges[i]);
    char hex[41];
    sha1_hex(key, hex);
    snprintf(p->filename, sizeof(p->filename), "cust-e-%s.dat", hex);
    snprintf(p->name, sizeof(p->name), "custom_edges_n%d", n);

    return p;
}

ptable_data_t* dsl_prune_make_corner_table(int* corners, int n) {
    if (n < 1 || n > 4) return NULL;

    custom_prune_table_ctx_t* ctx = calloc(1, sizeof(*ctx));
    ctx->kind = PRUNE_CORNERS;
    ctx->n = n;
    for (int i = 0; i < n; i++) {
        ctx->pieces[i] = corners[i];
        ctx->corner_mask |= (uint8_t)(1u << corners[i]);
    }
    ctx->edge_mask = 0;

    ptable_data_t* p = calloc(1, sizeof(*p));
    p->number_of_elements = (uint64_t)comb(NCORNERS, n)
                          * (uint64_t)pow(3, n)
                          * factorial(n);
    p->ptable_size = (p->number_of_elements + 1) / 2;
    
    p->cube_to_index_func = NULL;  // This should be cube_to_partial_c_index somehow
    p->read_value_ptable_func = ptable_read_val_2_values_per_byte;
    p->set_value_ptable_func = ptable_set_val_2_values_per_byte;
    p->is_custom = true;
    p->custom_data = ctx;
    p->ptable_is_loaded = false;
    p->ptable = NULL;
    p->moveset_mask = 0;
    p->gen_ptable_func = NULL;

    char key[128] = {0};
    int pos = 0;
    for (int i = 0; i < n; i++)
        pos += snprintf(key + pos, sizeof(key) - (size_t)pos, "%d,", corners[i]);
    char hex[41];
    sha1_hex(key, hex);
    snprintf(p->filename, sizeof(p->filename), "cust-c-%s.dat", hex);
    snprintf(p->name, sizeof(p->name), "custom_corners_n%d", n);

    return p;
}


/* ------------------------------------------------------------------ */
/* init / decompose for table_prune_gen                                */
/* ------------------------------------------------------------------ */

uint64_t dsl_prune_init_edges(ptable_gen_ctx_t* ctx, cube_t* cube,
                              uint64_t* components) {
    custom_prune_table_ctx_t* ct =
        (custom_prune_table_ctx_t*)ctx->ptable_data->custom_data;
    uint64_t eofb = cube_to_partial_eo_index(cube, UD, ct->pieces, ct->n);
    uint64_t epc  = cube_to_partial_epc_index(cube, UD, ct->pieces, ct->n);
    uint64_t epp  = cube_to_partial_epp_index(cube, UD, ct->pieces, ct->n);
    components[0] = eofb;
    components[1] = epc;
    components[2] = epp;
    return partial_eofb_epc_epp_to_partial_e_index(eofb, epc, epp, ct->n);
}

uint64_t dsl_prune_init_corners(ptable_gen_ctx_t* ctx, cube_t* cube,
                                uint64_t* components) {
    custom_prune_table_ctx_t* ct =
        (custom_prune_table_ctx_t*)ctx->ptable_data->custom_data;
    uint64_t coud = cube_to_partial_co_index(cube, UD, ct->pieces, ct->n);
    uint64_t cpc  = cube_to_partial_cpc_index(cube, UD, ct->pieces, ct->n);
    uint64_t cpp  = cube_to_partial_cpp_index(cube, UD, ct->pieces, ct->n);
    components[0] = coud;
    components[1] = cpc;
    components[2] = cpp;
    return partial_coud_cpc_cpp_to_partial_c_index(coud, cpc, cpp, ct->n);
}

void dsl_prune_decompose_edges(ptable_gen_ctx_t* ctx, uint64_t index,
                               uint64_t* components) {
    custom_prune_table_ctx_t* ct =
        (custom_prune_table_ctx_t*)ctx->ptable_data->custom_data;
    uint64_t two_n = 1ULL << (unsigned)ct->n;
    uint64_t cn    = (uint64_t)comb(NEDGES, ct->n);
    components[0] = index % two_n;
    components[1] = (index / two_n) % cn;
    components[2] = (index / two_n) / cn;
}

void dsl_prune_decompose_corners(ptable_gen_ctx_t* ctx, uint64_t index,
                                 uint64_t* components) {
    custom_prune_table_ctx_t* ct =
        (custom_prune_table_ctx_t*)ctx->ptable_data->custom_data;
    uint64_t three_n = (uint64_t)pow(3, ct->n);
    uint64_t cn      = (uint64_t)comb(NCORNERS, ct->n);
    components[0] = index % three_n;
    components[1] = (index / three_n) % cn;
    components[2] = (index / three_n) / cn;
}

/* ------------------------------------------------------------------ */
/* AST walk: collect tables from solved: atoms                         */
/* ------------------------------------------------------------------ */

/* Forward declaration. */
static void collect_solved_tables(const dsl_expr_t* e,
                                  ptable_data_t*** arr, uint8_t* n, uint8_t* cap);

static void collect_from_atom(const dsl_expr_t* a,
                              ptable_data_t*** arr, uint8_t* n, uint8_t* cap) {
    if (a->atom_kind != ATOM_SOLVED) return;

    const int edge_group_size = 3;
    const int corner_group_size = 4;

    /* split edge_mask into groups of <= edge_group_size */
    int ebits[16];
    int ne = extract_set_bits_u16(a->edge_mask, ebits);

    for (int i = 0; i < ne; i += edge_group_size) {
        int chunk = (ne - i) < edge_group_size ? (ne - i) : edge_group_size;
        ptable_data_t* t = dsl_prune_make_edge_table(ebits + i, chunk);
        if (*n == *cap) {
            *cap = *cap ? (uint8_t)(*cap * 2) : edge_group_size;
            *arr = realloc(*arr, (size_t)*cap * sizeof(ptable_data_t*));
        }
        (*arr)[(*n)++] = t;
    }

    /* split corner_mask into groups of <= corner_group_size */
    int cbits[8];
    int nc = extract_set_bits_u8(a->corner_mask, cbits);

    for (int i = 0; i < nc; i += corner_group_size) {
        int chunk = (nc - i) < corner_group_size ? (nc - i) : corner_group_size;
        ptable_data_t* t = dsl_prune_make_corner_table(cbits + i, chunk);
        if (*n == *cap) {
            *cap = *cap ? (uint8_t)(*cap * 2) : corner_group_size;
            *arr = realloc(*arr, (size_t)*cap * sizeof(ptable_data_t*));
        }
        (*arr)[(*n)++] = t;
    }
}

static void collect_solved_tables(const dsl_expr_t* e,
                                  ptable_data_t*** arr, uint8_t* n, uint8_t* cap) {
    if (!e) return;
    switch (e->kind) {
    case EXPR_AND:
    case EXPR_OR:
    case EXPR_PROD:
        collect_solved_tables(e->left, arr, n, cap);
        collect_solved_tables(e->right, arr, n, cap);
        break;
    case EXPR_NOT:
        break;
    case EXPR_ATOM:
        collect_from_atom(e, arr, n, cap);
        break;
    }
}

/* ------------------------------------------------------------------ */
/* the public make_ptables function                                    */
/* ------------------------------------------------------------------ */

ptable_data_t** dsl_prune_make_ptables(dsl_expr_t* expr, uint8_t* n_out) {
    ptable_data_t** arr = NULL;
    uint8_t n = 0, cap = 0;

    collect_solved_tables(expr, &arr, &n, &cap);

    if (n == 0) {
        *n_out = 0;
        return NULL;
    }

    /* build per-table contexts array */
    custom_prune_table_ctx_t* table_ctxs = malloc(n * sizeof(custom_prune_table_ctx_t));
    for (uint8_t i = 0; i < n; i++)
        table_ctxs[i] = *(custom_prune_table_ctx_t*)arr[i]->custom_data;

    /* build the step context, stored in table 0's custom_data */
    dsl_prune_step_ctx_t* step_ctx = calloc(1, sizeof(*step_ctx));
    step_ctx->expr = expr;
    step_ctx->tables = arr;
    step_ctx->n_tables = n;
    step_ctx->table_ctxs = table_ctxs;

    /* replace table 0's custom_data with the step context */
    free(arr[0]->custom_data);
    arr[0]->custom_data = step_ctx;

    *n_out = n;
    return arr;
}

/* ------------------------------------------------------------------ */
/* the heuristic: DSL AST with leaves swapped for tables               */
/* ------------------------------------------------------------------ */

static bool mask_is_subset(uint16_t child_edge, uint8_t child_corner,
                           uint16_t atom_edge, uint8_t atom_corner) {
    return (child_edge & atom_edge) == child_edge
        && (child_corner & atom_corner) == child_corner;
}

static bool prod_side_is_set(const dsl_expr_t* e) {
    return e->kind == EXPR_ATOM && e->atom_kind == ATOM_SET;
}

static size_t eval_prune_expr(const dsl_expr_t* e, cube_t* cube,
                              dsl_prune_step_ctx_t* ctx) {
    if (e == NULL){
        printf("e == NULL. Returning 0\n");
        return 0;
    }

    switch (e->kind) {
    case EXPR_NOT:
        return 0;

    case EXPR_AND: {
        size_t l = eval_prune_expr(e->left, cube, ctx);
        size_t r = eval_prune_expr(e->right, cube, ctx);
        return l > r ? l : r;
    }
    case EXPR_OR: {
        size_t l = eval_prune_expr(e->left, cube, ctx);
        size_t r = eval_prune_expr(e->right, cube, ctx);
        return l < r ? l : r;
    }
    case EXPR_PROD: {
        /* An admissible bound exists only with the set operand on the
         * LEFT: if c*s = h*p then h^-1*c*s = p, hence
         *   min_h dist_P(h^-1*c) <= dist(c, H*P).
         * With the set on the RIGHT the mirrored transform c*h^-1 is
         * inadmissible: the inverse would land between the solution
         * moves and the goal, inflating the bound. No pruning there.
         * Membership in dsl_eval handles both directions correctly. */
        if (!prod_side_is_set(e->left)) return 0;
        size_t best = (size_t)-1;
        for (int i = 0; i < e->left->subgroup_len; i++) {
            cube_t inv = cube_operation_inverse(e->left->subgroup_elems[i]);
            cube_t transformed = cube_operation_compose(inv, *cube);
            size_t v = eval_prune_expr(e->right, &transformed, ctx);
            if (v < best) best = v;
        }
        return best == (size_t)-1 ? 0 : best;
    }
    case EXPR_ATOM: {
        if (e->atom_kind != ATOM_SOLVED) return 0;
        size_t best = 0;
        for (uint8_t i = 0; i < ctx->n_tables; i++) {
            custom_prune_table_ctx_t* tc = &ctx->table_ctxs[i];
            if (!mask_is_subset(tc->edge_mask, tc->corner_mask,
                                e->edge_mask, e->corner_mask))
                continue;
            ptable_data_t* t = ctx->tables[i];
            if (!t->ptable_is_loaded || !t->ptable) continue;
            uint64_t idx;
            if (t->is_custom) {
                if (tc->kind == PRUNE_EDGES)
                    idx = cube_to_partial_e_index(cube, UD, tc->pieces, tc->n);
                else
                    idx = cube_to_partial_c_index(cube, UD, tc->pieces, tc->n);
            } else {
                idx = t->cube_to_index_func(cube, UD);
            }
            size_t v = (size_t)t->read_value_ptable_func(idx, (uint8_t*)t->ptable);
            if (v > best) best = v;
        }
        return best;
    }
    }
    return 0;
}

size_t dsl_prune_heuristic(cube_t* c, solving_step* ss) {
    ptable_data_t* p_data = ss->p_data;
    dsl_prune_step_ctx_t* ctx = (dsl_prune_step_ctx_t*)p_data->custom_data;
    if (!ctx || !ctx->expr) return 0;
    return eval_prune_expr(ctx->expr, c, ctx);
}

/* ------------------------------------------------------------------ */
/* free                                                                */
/* ------------------------------------------------------------------ */

void dsl_prune_free_tables(ptable_data_t** tables, uint8_t n) {
    if (!tables) return;

    /* recover the step context from table 0 */
    dsl_prune_step_ctx_t* step_ctx = NULL;
    if (n > 0 && tables[0]->custom_data) {
        step_ctx = (dsl_prune_step_ctx_t*)tables[0]->custom_data;
    }

    for (uint8_t i = 0; i < n; i++) {
        ptable_data_t* t = tables[i];
        if (!t) continue;

        if (t->ptable_is_loaded && t->ptable) {
            /* mmap'd from disk */
            free_ptable(t);
        }

        /* free custom_data: for table 0 it's the step_ctx (freed later);
         * for tables 1..n-1 it's a per-table context */
        if (i == 0) {
            /* step_ctx freed below */
        } else {
            free(t->custom_data);
        }

        free(t);
    }

    if (step_ctx) {
        free(step_ctx->table_ctxs);
        free(step_ctx);
    }

    free(tables);
}

