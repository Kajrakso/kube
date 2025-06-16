#include "criterion/criterion.h"
#include "criterion/redirect.h"

#include "../src/index.h"

// suits

TestSuite(cube_index);
TestSuite(index_bounds);

// tests

Test(cube_index, c_index_is_section) {
  cube_tables_generate();
  precompute_combinatorials();

  for (uint32_t i = 0; i < 10000; i++) {
    cube_t c = c_index_to_cube(i);
    uint32_t j = cube_to_c_index(&c);
    cr_assert_eq(i, j);
  }
}
