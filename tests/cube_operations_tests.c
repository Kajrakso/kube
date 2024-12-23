#include "../include/libcube.h"
#include "criterion/criterion.h"
#include "criterion/redirect.h"
#include <stdint.h>
#include <stdio.h>

// suites

TestSuite(cube_operations);

// tests


Test(cube_operations, cube_equality){
    cube_t cube = cube_create_new_cube();
    cube_t cube_copy = cube_create_new_cube();
    cube_movetables_generate();

    cr_assert(cube_operation_is_equal(cube, cube_copy));

    cube_move_apply_move(&cube, R1);
    cube_move_apply_move(&cube_copy, R1);

    cr_assert(cube_operation_is_equal(cube, cube_copy));

    cube_move_apply_move(&cube, U1);
    cube_move_apply_move(&cube_copy, U3);

    cr_assert_not(cube_operation_is_equal(cube, cube_copy));
}

Test(cube_operations, inverse){
    cube_movetables_generate();

    cube_t cube = cube_create_new_cube();
    cube_t cube_inverse = cube_operation_inverse(cube);

    cr_assert(cube_operation_is_equal(cube, cube_inverse));  // identity is its own inverse

    cube_move_apply_move(&cube, R1);
    cube_move_apply_move(&cube_inverse, R3);

    cr_assert(cube_operation_is_equal(cube_operation_inverse(cube), cube_inverse));

    cube_t cube2 = cube_create_new_cube();
    cube_t cube2_inverse = cube_create_new_cube();

    cube_move_apply_move_string(&cube2, "R U L F");
    cube_move_apply_move_string(&cube2_inverse, "F' L' U' R'");

    cr_assert(cube_operation_is_equal(cube_operation_inverse(cube2), cube2_inverse));

    cube_t cube3 = cube_create_new_cube();
    cube_t cube3_inverse = cube_create_new_cube();

    cube_move_apply_move(&cube3, R1);

    cr_assert_not(cube_operation_is_equal(cube_operation_inverse(cube3), cube3_inverse));

    cube_t cube4 = cube_create_new_cube();
    cube_t cube4_inverse = cube_operation_inverse(cube4);

    cube_move_apply_move_string(&cube4, "B R U L");
    cube_move_apply_move_string(&cube4_inverse, "R U F L'");

    cr_assert_not(cube_operation_is_equal(cube_operation_inverse(cube4), cube4_inverse));
}