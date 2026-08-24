#include "dsl_compile.h"
#include "cli.h"
#include "moveset.h"
#include "dsl_prune.h"

/* ------------------------
 * functions that bridges the gap
 * between custom solving step
 * and the predefined steps
 * ------------------------ */

static bool custom_cube_is_solved(cube_t* c, void* data) {
    dsl_expr_t* e = data;
    return dsl_eval(e, c);
}

static size_t generic_ptable_heuristic(cube_t* c, solving_step* ss) {
    uint64_t idx = ss->p_data->cube_to_index_func(c, UD);
    return ss->p_data->read_value_ptable_func(idx, ss->p_data->ptable);
}

/* ------------------------
 * Read the dsl_defs from arguments,
 * parse them and compile the custom
 * solving_steps. This is where
 * which cube_is_solved_* function
 * and which pruning table to use
 * is wired together.
 * ------------------------ */
int dsl_compile_build_definitions(dsl_compile_definition_t** defs, int* n_defs, struct arguments* arguments){
    if (defs == NULL) {
        fprintf(stderr, "defs = NULL\n");
        return 1;
    }

    if (*defs != NULL) dsl_compile_free_definitions(*defs, n_defs);
    *n_defs = arguments->def_count;
    *defs = calloc((size_t)arguments->def_count, sizeof **defs);
    
    if (arguments->verbose == 1) {
        fprintf(stderr, "Building %i definitions\n", arguments->def_count);
    }
    for (int i = 0; i < arguments->def_count; i++) {
        dsl_compile_definition_t* df = &(*defs)[i];
        df->name = strdup(arguments->defs[i].name);
        df->moveset_mask = arguments->defs[i].moveset_mask;
   
        char err[256];
        df->expr = dsl_parse(arguments->defs[i].expr, err, sizeof err);
        if (!df->expr) {
            fprintf(stderr, "could not parse definition '%s': %s\n", df->name, err);
            return 1;
        }
        
        if (arguments->verbose == 1) {
            dsl_debug_print(df->expr, stdout);
        }
        df->canonical = dsl_canonical(df->expr);

        /* solve everything  ==  fin  (keeps the fast IDA_fin + opt1 path) */
        if (dsl_is_maybe_fin(df->expr)) {
            if (moveset_eff(df->moveset_mask) == MOVESET_HTM) {
                df->step = fin;
                df->step.name = df->name;       /* keep the user's name for messages */
                df->step.is_custom = false;     /* do not free: shares static ptable */
                continue;
            }
            df->step.name = df->name;
            df->step.is_custom = true;
            df->step.solving_type = SOLVE_CUSTOM;
            df->step.cube_is_solved = custom_cube_is_solved;
            df->step.custom_data = df->expr;
            df->step.p_data = &ptable_data_opt1;
            df->step.heuristic_func = generic_ptable_heuristic;
            df->step.moveset_mask = df->moveset_mask;
            continue;
        }
   
        df->step.name = df->name;
        df->step.is_custom = true;
        df->step.solving_type = SOLVE_CUSTOM;
        df->step.cube_is_solved = custom_cube_is_solved;
        df->step.custom_data = df->expr;
        df->step.moveset_mask = df->moveset_mask;

        /* build custom pruning tables from the DSL expression */
        df->step.custom_ptables =
            dsl_prune_make_ptables(df->expr, &df->step.n_custom_ptables);
        if (df->step.custom_ptables != NULL && df->step.n_custom_ptables > 0) {
            df->step.p_data = df->step.custom_ptables[0];
            df->step.heuristic_func = dsl_prune_heuristic;
        } else {
            df->step.custom_ptables = NULL;
            df->step.n_custom_ptables = 0;
            df->step.p_data = NULL;
            df->step.heuristic_func = NULL;
        }
    }
    return 0;
}

void dsl_compile_free_definitions(dsl_compile_definition_t* defs, int* n_defs) {
    if (defs == NULL) return;
    if (n_defs == NULL) return;
    int number_of_defs = *n_defs;

    for (int i = 0; i < number_of_defs; i++) {
        dsl_compile_definition_t* df = &defs[i];
        free(df->name);
        free(df->canonical);

        /* Free custom pruning tables (general custom steps) */
        if (df->step.is_custom && df->step.custom_ptables != NULL) {
            dsl_prune_free_tables(df->step.custom_ptables, df->step.n_custom_ptables);
        }

        /* For custom steps reusing the shared opt1 ptable (p_data == &ptable_data_opt1),
         * nothing is owned here. Any other heap p_data belongs to custom_ptables,
         * freed above. */

        dsl_free_expression(df->expr);
    }

    free(defs);

    defs = NULL;
    *n_defs = 0;
}

