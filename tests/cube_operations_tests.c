#include <stdint.h>
#include <stdio.h>

#include "../include/libcube.h"
#include "criterion/criterion.h"
#include "criterion/redirect.h"

// suites

TestSuite(cube_operations);

// tests

Test(cube_operations, cube_equality) {
  cube_t cube = cube_create_new_cube();
  cube_t cube_copy = cube_create_new_cube();
  cube_tables_generate();

  cr_assert(cube_operation_is_equal(cube, cube_copy));

  cube_move_apply_move(&cube, R1);
  cube_move_apply_move(&cube_copy, R1);

  cr_assert(cube_operation_is_equal(cube, cube_copy));

  cube_move_apply_move(&cube, U1);
  cube_move_apply_move(&cube_copy, U3);

  cr_assert_not(cube_operation_is_equal(cube, cube_copy));
}

Test(cube_operations, inverse) {
  cube_tables_generate();

  cube_t cube = cube_create_new_cube();
  cube_t cube_inverse = cube_operation_inverse(cube);

  cr_assert(cube_operation_is_equal(
      cube, cube_inverse));  // identity is its own inverse

  cube_move_apply_move(&cube, R1);
  cube_move_apply_move(&cube_inverse, R3);

  cr_assert(
      cube_operation_is_equal(cube_operation_inverse(cube), cube_inverse));

  cube_t cube2 = cube_create_new_cube();
  cube_t cube2_inverse = cube_create_new_cube();

  cube_move_apply_move_string(&cube2, "R U L F");
  cube_move_apply_move_string(&cube2_inverse, "F' L' U' R'");

  cr_assert(
      cube_operation_is_equal(cube_operation_inverse(cube2), cube2_inverse));

  cube_t cube3 = cube_create_new_cube();
  cube_t cube3_inverse = cube_create_new_cube();

  cube_move_apply_move(&cube3, R1);

  cr_assert_not(
      cube_operation_is_equal(cube_operation_inverse(cube3), cube3_inverse));

  cube_t cube4 = cube_create_new_cube();
  cube_t cube4_inverse = cube_operation_inverse(cube4);

  cube_move_apply_move_string(&cube4, "B R U L");
  cube_move_apply_move_string(&cube4_inverse, "R U F L'");

  cr_assert_not(
      cube_operation_is_equal(cube_operation_inverse(cube4), cube4_inverse));
}

Test(cube_operations, composition) {
  cube_tables_generate();

  cube_t cube1 = cube_create_new_cube();
  cube_t cube2 = cube_create_new_cube();
  cube_t cube_result = cube_create_new_cube();

  cube_move_apply_move(&cube1, R1);
  cube_move_apply_move(&cube2, U1);

  cube_move_apply_move_string(&cube_result, "R U");

  cr_assert(cube_operation_is_equal(cube_operation_compose(cube1, cube2),
                                    cube_result));

  cube_t cube_1 = cube_create_new_cube();
  cube_t cube_2 = cube_create_new_cube();

  cr_assert(cube_operation_is_equal(cube_operation_compose(cube_1, cube_2),
                                    cube_create_new_cube()));
}

Test(cube_operations, composition2) {
  cube_tables_generate();

  cube_t cube1 = cube_create_new_cube();
  cube_t cube2 = cube_create_new_cube();
  cube_t cube_result = cube_create_new_cube();

  cube_move_apply_move_string(&cube1, "F L F D' R' F2 B' R2 U'");
  cube_move_apply_move_string(&cube2, "B2 R' D2 R' F2 L D2 R U2 L2 D2 L");
  cube_move_apply_move_string(
      &cube_result, "F L F D' R' F2 B' R2 U' B2 R' D2 R' F2 L D2 R U2 L2 D2 L");

  cr_assert(cube_operation_is_equal(cube_operation_compose(cube1, cube2),
                                    cube_result));

  cube1 = cube_create_new_cube();
  cube2 = cube_create_new_cube();
  cube_result = cube_create_new_cube();

  cube_move_apply_move_string(&cube1, "U2 D' B R2 U' F B2 L B'");
  cube_move_apply_move_string(&cube2, "R2 D2 R2 F2 D F2 D2 B2 R2 B2 D");
  cube_move_apply_move_string(
      &cube_result, "U2 D' B R2 U' F B2 L B' R2 D2 R2 F2 D F2 D2 B2 R2 B2 D");

  cr_assert(cube_operation_is_equal(cube_operation_compose(cube1, cube2),
                                    cube_result));

  cube1 = cube_create_new_cube();
  cube2 = cube_create_new_cube();
  cube_result = cube_create_new_cube();

  cube_move_apply_move_string(&cube1, "F2 B R L2 F L' U' D' L F2 L2 U2");
  cube_move_apply_move_string(&cube2, "F' R2 D2 B2 U2 F' L2 B2 D2");
  cube_move_apply_move_string(
      &cube_result,
      "F2 B R L2 F L' U' D' L F2 L2 U2 F' R2 D2 B2 U2 F' L2 B2 D2");

  cr_assert(cube_operation_is_equal(cube_operation_compose(cube1, cube2),
                                    cube_result));

  cube1 = cube_create_new_cube();
  cube2 = cube_create_new_cube();
  cube_result = cube_create_new_cube();

  cube_move_apply_move_string(&cube1, "B' L B2 R' U R' D' B2 R' F'");
  cube_move_apply_move_string(&cube2, "U2 L2 U' F2 U L2 F2 D B2 D' F2 L2");
  cube_move_apply_move_string(
      &cube_result,
      "B' L B2 R' U R' D' B2 R' F' U2 L2 U' F2 U L2 F2 D B2 D' F2 L2");

  cr_assert(cube_operation_is_equal(cube_operation_compose(cube1, cube2),
                                    cube_result));
}
