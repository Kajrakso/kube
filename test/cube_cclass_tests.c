#include "criterion/criterion.h"

#include <stdint.h>

#include "../src/scrambler.h"
#include "../src/core/cube.h"
#include "../src/index.h"

Test(cclass, cclass_invariant_under_UDsyms) {
    static int UDsyms[] = {
      0,  1,  2,  3,  16, 17, 18, 19,  // no inversion
      24, 25, 26, 27, 40, 41, 42, 43   // inversion
    };

    for (int k = 0; k < 100; k++)
    {
        cube_t cube = cube_scrambler_get_handscrambled_cube();

        uint32_t ci = cclass_table[cube_to_c_index(&cube, UD)].cclass_i;
        uint32_t c  = cclass_table[cube_to_c_index(&cube, UD)].cclass;
        uint32_t ci2, c2;

        for (int i = 1; i < 16; i++)
        {
            int    t     = UDsyms[i];
            cube_t cube2 = cube_operation_sym_conjugate(cube, t);

            ci2 = cclass_table[cube_to_c_index(&cube2, UD)].cclass_i;
            c2  = cclass_table[cube_to_c_index(&cube2, UD)].cclass;

            cr_assert_eq(ci, ci2);
            cr_assert_eq(c2, c2);
        }
    }
}
