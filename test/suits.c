#include "criterion/criterion.h"

#include "../src/tables.h"
#include "../src/solver_pipeline.h"
#include <time.h>

/* runs before each test */
void setup(void) {
    srand((unsigned int) time(NULL));
    init_env();
    cube_tables_generate();
}

/* runs after each test */
void teardown(void) {
}

/* =====================
 * SETUP FOR SPECIFIC SUITS
 * ====================== */

void setup_index(void) {
    srand((unsigned int) time(NULL));
    init_env();

    cube_tables_generate();
    cube_tables_load();
}


void setup_solver_fin(void) {
    srand((unsigned int) time(NULL));
    init_env();

    cube_tables_generate();
    cube_tables_load();

    solving_step* ss = &fin;
    cube_tables_load_ptable(ss->p_data);
}

void teardown_solver_fin(void){
    cube_tables_free();

    solving_step* ss = &fin;
    free_ptable(ss->p_data);
}

void teardown_index(void) {
    cube_tables_free();
}

void setup_dsl_ptable(void) {
    srand((unsigned int) time(NULL));
    init_env();
    cube_tables_generate();
}

void teardown_dsl_ptable(void) {
}

/* =====================
 * SUITS
 * ====================== */

// enabled

TestSuite(solutions, .disabled=false);
TestSuite(cube_repr, .disabled=false);
TestSuite(util_tests, .disabled=false);
TestSuite(scramble, .disabled=false, .init=setup);
TestSuite(cube_info, .disabled=false, .init=setup, .fini=teardown);
TestSuite(cube_operations, .disabled=false, .init=setup, .fini=teardown);
TestSuite(mtables, .disabled=false, .init=setup, .fini=teardown);
TestSuite(standard_cube_moves, .disabled=false, .init=setup, .fini=teardown);
TestSuite(cube_index, .disabled=false, .init=setup_index, .fini=teardown_index);
TestSuite(fix_orientation, .disabled=false, .init=setup, .fini=teardown);
TestSuite(cclass, .disabled=false, .init=setup, .fini=teardown);

TestSuite(sha1, .disabled=false);
TestSuite(dsl_parse, .disabled=false);
TestSuite(dsl_eval, .disabled=false, .init=setup_dsl_ptable, .fini=teardown_dsl_ptable);
TestSuite(dsl_equiv, .disabled=false, .init=setup_dsl_ptable, .fini=teardown_dsl_ptable);

TestSuite(dsl_debug, .disabled=false);

TestSuite(moveset, .disabled=false, .init=setup, .fini=teardown);
TestSuite(dsl_set, .disabled=false, .init=setup, .fini=teardown);
TestSuite(dsl_prod, .disabled=false, .init=setup, .fini=teardown);
TestSuite(dsl_ptable, .disabled=false, .init=setup_dsl_ptable, .fini=teardown_dsl_ptable);
TestSuite(dsl_compile, .disabled=false, .init=setup_solver_fin, .fini=teardown_solver_fin);

TestSuite(solver_fin, .disabled=false, .init=setup_solver_fin, .fini=teardown_solver_fin);
TestSuite(solver_multiple_solutions, .disabled=false, .init=setup_solver_fin, .fini=teardown_solver_fin);

// disabled

