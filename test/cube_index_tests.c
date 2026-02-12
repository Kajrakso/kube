#include "criterion/criterion.h"

#include "../src/index.h"
#include "../src/scrambler.h"

Test(cube_index, c_index_is_section) {
  for (uint32_t i = 0; i < 10000; i++) {
    cube_t c = c_index_to_cube(i);
    uint32_t j = cube_to_c_index(&c, UD);
    cr_assert_eq(i, j);
  }
}

Test(cube_index, H_lr_same_as_conj){
    for (int j = 0; j < 100; j++){
        cube_t c = cube_scrambler_get_handscrambled_cube();

        uint64_t i = cube_to_H_index(&c, LR);
        cube_t c2 = cube_operation_sym_conjugate(c, 13); // (y)^1 (y y z z z)^1
        uint64_t ii = cube_to_H_index(&c2, UD);

        cr_assert_eq(i, ii);
    }
}

Test(cube_index, H_fb_same_as_conj){
    for (int j = 0; j < 100; j++){
        cube_t c = cube_scrambler_get_handscrambled_cube();

        uint64_t i = cube_to_H_index(&c, FB);
        cube_t c2 = cube_operation_sym_conjugate(c, 10); // (y)^2 (y z)^2
        uint64_t ii = cube_to_H_index(&c2, UD);

        cr_assert_eq(i, ii);
    }
}
