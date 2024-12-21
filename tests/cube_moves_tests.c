#include "../include/libcube.h"
#include "../src/utils.h"
#include "../src/_cube.h"
#include "criterion/criterion.h"
#include "criterion/redirect.h"
#include <stdint.h>
#include <stdio.h>

// suites

TestSuite(cube_moves);

// tests

Test(cube_moves, single_U_move){
  generate_move_tables();

  cube_t cube = cube_create_new_cube();

  /* A U move changes the eo on UD. */
  uint16_t edges_after_U[NEDGES] = {
    build_edge(1, 0, 0, 1), build_edge(2, 0, 0, 1), build_edge(3, 0, 0, 1), build_edge(0, 0, 0, 1),
    build_edge(4, 0, 0, 0), build_edge(5, 0, 0, 0), build_edge(6, 0, 0, 0), build_edge(7, 0, 0, 0),
    build_edge(8, 0, 0, 0), build_edge(9, 0, 0, 0), build_edge(10, 0, 0, 0), build_edge(11, 0, 0, 0),
  };

  /* A U move changes co on FB and RL. */
  uint16_t corners_after_U[NCORNERS] = {
    build_corner(1, 1, 2, 0), build_corner(2, 2, 1, 0), build_corner(3, 1, 2, 0), build_corner(0, 2, 1, 0), 
    build_corner(4, 0, 0, 0), build_corner(5, 0, 0, 0), build_corner(6, 0, 0, 0), build_corner(7, 0, 0, 0), 
  };

  cube_apply_move(&cube, U1);

  cr_assert(arr_eq(cube.edges, edges_after_U, NEDGES));
  cr_assert(arr_eq(cube.corners, corners_after_U, NCORNERS));
}

Test(cube_moves, single_D_move){
  generate_move_tables();

  cube_t cube = cube_create_new_cube();

  /* A D move changes the eo on UD. */
  uint16_t edges_after_D[NEDGES] = {
    build_edge(0, 0, 0, 0), build_edge(1, 0, 0, 0), build_edge(2, 0, 0, 0), build_edge(3, 0, 0, 0),
    build_edge(5, 0, 0, 1), build_edge(6, 0, 0, 1), build_edge(7, 0, 0, 1), build_edge(4, 0, 0, 1),
    build_edge(8, 0, 0, 0), build_edge(9, 0, 0, 0), build_edge(10, 0, 0, 0), build_edge(11, 0, 0, 0),
  };

  /* A D move changes co on FB and RL. */
  uint16_t corners_after_D[NCORNERS] = {
    build_corner(0, 0, 0, 0), build_corner(1, 0, 0, 0), build_corner(2, 0, 0, 0), build_corner(3, 0, 0, 0), 
    build_corner(5, 1, 2, 0), build_corner(6, 2, 1, 0), build_corner(7, 1, 2, 0), build_corner(4, 2, 1, 0), 
  };

  cube_apply_move(&cube, D1);

  cr_assert(arr_eq(cube.edges, edges_after_D, NEDGES));
  cr_assert(arr_eq(cube.corners, corners_after_D, NCORNERS));
}

Test(cube_moves, single_R_move){
  generate_move_tables();

  cube_t cube = cube_create_new_cube();

  /* A R move changes the eo on RL. */
  uint16_t edges_after_R[NEDGES] = {
    build_edge(0, 0, 0, 0), build_edge(9, 0, 1, 0), build_edge(2, 0, 0, 0), build_edge(3, 0, 0, 0),
    build_edge(4, 0, 0, 0), build_edge(10, 0, 1, 0), build_edge(6, 0, 0, 0), build_edge(7, 0, 0, 0),
    build_edge(8, 0, 0, 0), build_edge(5, 0, 1, 0), build_edge(1, 0, 1, 0), build_edge(11, 0, 0, 0),
  };

  /* A R move changes co on FB and UD. */
  uint16_t corners_after_R[NCORNERS] = {
    build_corner(0, 0, 0, 0), build_corner(6, 1, 0, 2), build_corner(1, 2, 0, 1), build_corner(3, 0, 0, 0), 
    build_corner(4, 0, 0, 0), build_corner(2, 1, 0, 2), build_corner(5, 2, 0, 1), build_corner(7, 0, 0, 0),
  };

  cube_apply_move(&cube, R1);

  cr_assert(arr_eq(cube.edges, edges_after_R, NEDGES));
  cr_assert(arr_eq(cube.corners, corners_after_R, NCORNERS));
}

Test(cube_moves, single_L_move){
  generate_move_tables();

  cube_t cube = cube_create_new_cube();

  /* A L move changes the eo on RL. */
  uint16_t edges_after_L[NEDGES] = {
    build_edge(0, 0, 0, 0), build_edge(1, 0, 0, 0), build_edge(2, 0, 0, 0), build_edge(11, 0, 1, 0),
    build_edge(4, 0, 0, 0), build_edge(5, 0, 0, 0), build_edge(6, 0, 0, 0), build_edge(8, 0, 1, 0),
    build_edge(3, 0, 1, 0), build_edge(9, 0, 0, 0), build_edge(10, 0, 0, 0), build_edge(7, 0, 1, 0),
  };

  /* A L move changes co on FB and UD. */
  uint16_t corners_after_L[NCORNERS] = {
    build_corner(3, 2, 0, 1), build_corner(1, 0, 0, 0), build_corner(2, 0, 0, 0), build_corner(4, 1, 0, 2), 
    build_corner(7, 2, 0, 1), build_corner(5, 0, 0, 0), build_corner(6, 0, 0, 0), build_corner(0, 1, 0, 2),
  };

  cube_apply_move(&cube, L1);

  cr_assert(arr_eq(cube.edges, edges_after_L, NEDGES));
  cr_assert(arr_eq(cube.corners, corners_after_L, NCORNERS));
}

Test(cube_moves, single_F_move){
  generate_move_tables();

  cube_t cube = cube_create_new_cube();

  /* A F move changes the eo on FB. */
  uint16_t edges_after_F[NEDGES] = {
    build_edge(0, 0, 0, 0), build_edge(1, 0, 0, 0), build_edge(10, 1, 0, 0), build_edge(3, 0, 0, 0),
    build_edge(11, 1, 0, 0), build_edge(5, 0, 0, 0), build_edge(6, 0, 0, 0), build_edge(7, 0, 0, 0),
    build_edge(8, 0, 0, 0), build_edge(9, 0, 0, 0), build_edge(4, 1, 0, 0), build_edge(2, 1, 0, 0),
  };

  /* A F move changes co on RL and UD. */
  uint16_t corners_after_F[NCORNERS] = {
    build_corner(0, 0, 0, 0), build_corner(1, 0, 0, 0), build_corner(5, 0, 1, 2), build_corner(2, 0, 2, 1), 
    build_corner(3, 0, 1, 2), build_corner(4, 0, 2, 1), build_corner(6, 0, 0, 0), build_corner(7, 0, 0, 0),
  };

  cube_apply_move(&cube, F1);

  cr_assert(arr_eq(cube.edges, edges_after_F, NEDGES));
  cr_assert(arr_eq(cube.corners, corners_after_F, NCORNERS));
}

Test(cube_moves, single_B_move){
  generate_move_tables();

  cube_t cube = cube_create_new_cube();

  /* A B move changes the eo on FB. */
  uint16_t edges_after_B[NEDGES] = {
    build_edge(8, 1, 0, 0), build_edge(1, 0, 0, 0), build_edge(2, 0, 0, 0), build_edge(3, 0, 0, 0),
    build_edge(4, 0, 0, 0), build_edge(5, 0, 0, 0), build_edge(9, 1, 0, 0), build_edge(7, 0, 0, 0),
    build_edge(6, 1, 0, 0), build_edge(0, 1, 0, 0), build_edge(10, 0, 0, 0), build_edge(11, 0, 0, 0),
  };

  /* A B move changes co on RL and UD. */
  uint16_t corners_after_B[NCORNERS] = {
    build_corner(7, 0, 1, 2), build_corner(0, 0, 2, 1), build_corner(2, 0, 0, 0), build_corner(3, 0, 0, 0), 
    build_corner(4, 0, 0, 0), build_corner(5, 0, 0, 0), build_corner(1, 0, 1, 2), build_corner(6, 0, 2, 1),
  };

  cube_apply_move(&cube, B1);

  cr_assert(arr_eq(cube.edges, edges_after_B, NEDGES));
  cr_assert(arr_eq(cube.corners, corners_after_B, NCORNERS));
}