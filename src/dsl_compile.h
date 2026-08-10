#ifndef DSL_COMPILE_H
#define DSL_COMPILE_H

#include "dsl.h"
#include "solver_steps.h"
#include "cli.h"

/* After passing a string to the --define
 * argument, we compile it into a custom
 * solving_step definition. */

typedef struct {
    char*       name;
    dsl_expr_t* expr;
    char*       canonical;
    solving_step step;
    bool        referenced;
    uint32_t    moveset_mask;
} dsl_compile_definition_t;

int dsl_compile_build_definitions(dsl_compile_definition_t** defs, int* n_defs, struct arguments* arguments);

void dsl_compile_free_definitions(dsl_compile_definition_t* defs, int* n_defs);

#endif /* DSL_COMPILE_H */
