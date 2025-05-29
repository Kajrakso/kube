#include <stdint.h>
#include <stdio.h>

#include "../include/libcube.h"
#include "../src/core/_cube.h"
#include "criterion/criterion.h"
#include "criterion/redirect.h"

// suites

TestSuite(cube_operations_cycles);

// tests

Test(cube_operations_cycles, edge_cycles) {
  cube_tables_generate();

  cube_t cube = cube_create_new_cube();
  cube_t cube2 = cube_create_new_cube();

  int edge_cycle[] = {UF, RU, UB};

  cube_move_apply_edge_cycle(&cube, edge_cycle, 3);
  cube_move_apply_move_string(&cube2, "F' B L F' B L2 F B' U2 L' F B'");

  cr_assert(cube_operation_is_equal(cube, cube2));

  cube_move_apply_edge_cycle(&cube, edge_cycle, 3);
  cube_move_apply_move_string(&cube2, "F' B L F' B L2 F B' U2 L' F B'");

  cr_assert(cube_operation_is_equal(cube, cube2));

  cube_move_apply_edge_cycle(&cube, edge_cycle, 3);
  cube_move_apply_move_string(&cube2, "F' B L F' B L2 F B' U2 L' F B'");

  cr_assert(cube_operation_is_equal(cube, cube2));
}

Test(cube_operations_cycles, edge_cycles2) {
  cube_tables_generate();

  cube_t cube = cube_create_new_cube();
  cube_t cube2 = cube_create_new_cube();

  int edge_cycle[] = {UF, DF, UR, UB, LU};

  cube_move_apply_edge_cycle(&cube, edge_cycle, 5);
  cube_move_apply_move_string(
      &cube2,
      "U R L' B2 R L' D' R' L B2 R' L U2 F' B L F' B L2 F B' U2 L' F B' U2");

  cr_assert(cube_operation_is_equal(cube, cube2));

  cube_move_apply_edge_cycle(&cube, edge_cycle, 5);
  cube_move_apply_move_string(
      &cube2,
      "U R L' B2 R L' D' R' L B2 R' L U2 F' B L F' B L2 F B' U2 L' F B' U2");

  cr_assert(cube_operation_is_equal(cube, cube2));

  cube_move_apply_edge_cycle(&cube, edge_cycle, 5);
  cube_move_apply_move_string(
      &cube2,
      "U R L' B2 R L' D' R' L B2 R' L U2 F' B L F' B L2 F B' U2 L' F B' U2");

  cr_assert(cube_operation_is_equal(cube, cube2));
}

Test(cube_operations_cycles, corner_cycles) {
  cube_tables_generate();

  cube_t cube = cube_create_new_cube();
  cube_t cube2 = cube_create_new_cube();

  int corner_cycle[] = {UFR, RDF, FDL};

  cube_move_apply_corner_cycle(&cube, corner_cycle, 3);
  cube_move_apply_move_string(&cube2, "R U R' D R U' R' D'");

  cr_assert(cube_operation_is_equal(cube, cube2));

  cube_move_apply_corner_cycle(&cube, corner_cycle, 3);
  cube_move_apply_move_string(&cube2, "R U R' D R U' R' D'");

  cr_assert(cube_operation_is_equal(cube, cube2));

  cube_move_apply_corner_cycle(&cube, corner_cycle, 3);
  cube_move_apply_move_string(&cube2, "R U R' D R U' R' D'");

  cr_assert(cube_operation_is_equal(cube, cube2));
}
