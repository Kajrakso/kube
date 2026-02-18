#include "criterion/criterion.h"

#include "../src/tables.h"
#include <time.h>

/* runs before each test */
void setup(void) {
    srand((unsigned int) time(NULL));
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

void teardown_index(void) {
    cube_tables_free();
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

// disabled

