#include "criterion/criterion.h"

#include <stdint.h>

#include "../src/core/cube.h"
#include "../src/index.h"
#include "../src/scrambler.h"

Test(mtables, mtables) {
    gen_move_table_ece_index();
    gen_move_table_coud_index();
    gen_move_table_eofb_index();
    gen_move_table_ccu_index();

    for (int i = 0; i < 100; i++)
    {
        cube_t   c    = cube_scrambler_get_handscrambled_cube();
        uint32_t ece  = cube_to_ec_index(&c, UD);
        uint32_t eofb = cube_to_eo_index(&c, UD);
        uint32_t ccu  = cube_to_cc_index(&c, UD);
        uint32_t coud = cube_to_co_index(&c, UD);

        // make moves
        for (int m = 0; m < NMOVES; m++)
        {
            cube_t c2 = c;
            cube_move_apply_move(&c2, m);
            uint32_t ece_m  = cube_to_ec_index(&c2, UD);
            uint32_t eofb_m = cube_to_eo_index(&c2, UD);
            uint32_t ccu_m  = cube_to_cc_index(&c2, UD);
            uint32_t coud_m = cube_to_co_index(&c2, UD);

            uint32_t ece_mt  = move_table_ece_index[ece][m];
            uint32_t eofb_mt = move_table_eofb_index[eofb][m];
            uint32_t ccu_mt  = move_table_ccu_index[ccu][m];
            uint32_t coud_mt = move_table_coud_index[coud][m];

            cr_assert_eq(ece_m, ece_mt);
            cr_assert_eq(eofb_m, eofb_mt);
            cr_assert_eq(ccu_m, ccu_mt);
            cr_assert_eq(coud_m, coud_mt);
        }
    }
}
