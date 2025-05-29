#include "../include/libcube.h"
#include "../src/solvers/_index.h"
#include "criterion/criterion.h"
#include "criterion/redirect.h"

// suits

TestSuite(cube_index);

// tests

Test(cube_index, c_index_is_section) {
  cube_tables_generate();
  precompute_combinatorials();

  bool fail = false;
  for (uint32_t i = 0; i < 10000; i++) {
    cube_t c = c_index_to_cube(i);
    uint32_t j = cube_to_c_index(&c);
    if (i != j) {
      fail = true;
      break;
    }
  }

  cr_assert(fail == false);
}
