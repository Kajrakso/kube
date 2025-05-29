#include <stdint.h>
#include <stdio.h>

#include "../include/libcube.h"
#include "../src/core/_cube.h"
#include "../src/core/_cube_state.h"
#include "../src/solvers/_index.h"
#include "../src/solvers/_solvers.h"
#include "../src/utils.h"
#include "criterion/criterion.h"
#include "criterion/redirect.h"

// suites

TestSuite(mtables);

// tests

Test(mtables, mtables) {
  cube_tables_generate();
  precompute_combinatorials();  // need to precompute these at the moment.
  gen_move_table_ece_index();
  gen_move_table_coud_index();
  gen_move_table_eofb_index();
  gen_move_table_ccu_index();

  for (int i = 0; i < 100; i++) {
    cube_t c = cube_scrambler_get_handscrambled_cube();
    uint32_t ece = cube_to_ece_index(&c);
    uint32_t eofb = cube_to_eofb_index(&c);
    uint32_t ccu = cube_to_ccu_index(&c);
    uint32_t coud = cube_to_coud_index(&c);

    // make moves
    for (int m = 0; m < NMOVES; m++) {
      cube_t c2 = c;
      cube_move_apply_move(&c2, m);
      uint32_t ece_m = cube_to_ece_index(&c2);
      uint32_t eofb_m = cube_to_eofb_index(&c2);
      uint32_t ccu_m = cube_to_ccu_index(&c2);
      uint32_t coud_m = cube_to_coud_index(&c2);

      uint32_t ece_mt = move_table_ece_index[ece][m];
      uint32_t eofb_mt = move_table_eofb_index[eofb][m];
      uint32_t ccu_mt = move_table_ccu_index[ccu][m];
      uint32_t coud_mt = move_table_coud_index[coud][m];

      cr_assert_eq(ece_m, ece_mt);
      cr_assert_eq(eofb_m, eofb_mt);
      cr_assert_eq(ccu_m, ccu_mt);
      cr_assert_eq(coud_m, coud_mt);
    }
  }
}
