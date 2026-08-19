#ifndef DSL_PRUNE_H
#define DSL_PRUNE_H

#include "dsl.h"
#include "tables.h"
#include "utils/utils.h"
#include "utils/sha1.h"

enum {
    PRUNE_EDGES   = 0,
    PRUNE_CORNERS = 1,
};

typedef struct {
    int         kind;       /* PRUNE_EDGES or PRUNE_CORNERS */
    int         n;          /* number of tracked pieces (<= 4) */
    int         pieces[4];  /* piece IDs (edge or corner indices), ascending */
    uint16_t    edge_mask;  /* bitmask of tracked edges (for matching to atoms) */
    uint8_t     corner_mask;/* bitmask of tracked corners (for matching to atoms) */
} custom_prune_table_ctx_t;

typedef struct {
    dsl_expr_t*              expr;       /* the DSL expression (for AST walking) */
    ptable_data_t**          tables;     /* array of table pointers */
    uint8_t                  n_tables;   /* number of tables */
    custom_prune_table_ctx_t* table_ctxs; /* array of per-table contexts (same order) */
} dsl_prune_step_ctx_t;

/* Create a ptable_data_t for n solved edges.
 * The table is NOT generated — call table_prune_gen separately. */
ptable_data_t* dsl_prune_make_edge_table(int* edges, int n);

/* Create a ptable_data_t for n solved corners. */
ptable_data_t* dsl_prune_make_corner_table(int* corners, int n);

/* Walk a DSL expression, extract solved: atoms, split into groups of <= 4,
 * create one ptable_data_t per group. Returns the array and count.
 * Tables are NOT generated — call table_prune_gen for each. */
ptable_data_t** dsl_prune_make_ptables(dsl_expr_t* expr, uint8_t* n_out);

/* Heuristic function for multi-table custom steps.
 * Walks the DSL AST and evaluates over the tables. */
size_t dsl_prune_heuristic(cube_t* c, ptable_data_t* p_data);

/* Free a custom step's tables and contexts. */
void dsl_prune_free_tables(ptable_data_t** tables, uint8_t n);


uint64_t dsl_prune_init_edges(ptable_gen_ctx_t*, cube_t*, uint64_t*);
uint64_t dsl_prune_init_corners(ptable_gen_ctx_t*, cube_t*, uint64_t*);
void dsl_prune_decompose_edges(ptable_gen_ctx_t*, uint64_t, uint64_t*);
void dsl_prune_decompose_corners(ptable_gen_ctx_t*, uint64_t, uint64_t*);

bool dsl_prune_generate_ptables();

#endif /* DSL_PRUNE_H */
