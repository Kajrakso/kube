#include "../src/cube.h"
#include "../src/utilities.h"
#include "../src/moves.h"
#include "criterion/criterion.h"
#include "criterion/redirect.h"
#include <stdint.h>
#include <stdio.h>

// suites

TestSuite(cube_moves, .disabled=false);

// tests

Test(cube_moves, init_mtable){
  cube_t solved_cube = cube_create_new_cube();
  cube_t cube = cube_create_new_cube();

  uint16_t c_trans[NMOVES][NCORNERCUBIES];
  uint16_t e_trans[NMOVES][NEDGECUBIES];

  initialize_move_tables(c_trans, e_trans);

  make_move(&cube, R, 0, c_trans, e_trans);
  cr_assert(arr_eq(cube.edges, solved_cube.edges, 12));
  cr_assert(arr_eq(cube.corners, solved_cube.corners, 8));
  
  make_move(&cube, L, 0, c_trans, e_trans);
  cr_assert(arr_eq(cube.edges, solved_cube.edges, 12));
  cr_assert(arr_eq(cube.corners, solved_cube.corners, 8));
  
  make_move(&cube, U, 0, c_trans, e_trans);
  cr_assert(arr_eq(cube.edges, solved_cube.edges, 12));
  cr_assert(arr_eq(cube.corners, solved_cube.corners, 8));
  
  make_move(&cube, D, 0, c_trans, e_trans);
  cr_assert(arr_eq(cube.edges, solved_cube.edges, 12));
  cr_assert(arr_eq(cube.corners, solved_cube.corners, 8));
  
  make_move(&cube, F, 0, c_trans, e_trans);
  cr_assert(arr_eq(cube.edges, solved_cube.edges, 12));
  cr_assert(arr_eq(cube.corners, solved_cube.corners, 8));
  
  make_move(&cube, B, 0, c_trans, e_trans);
  cr_assert(arr_eq(cube.edges, solved_cube.edges, 12));
  cr_assert(arr_eq(cube.corners, solved_cube.corners, 8));

    make_move(&cube, R, 1, c_trans, e_trans);
  cr_assert(arr_eq(cube.edges, solved_cube.edges, 12));
  cr_assert(arr_eq(cube.corners, solved_cube.corners, 8));
  
  make_move(&cube, L, 1, c_trans, e_trans);
  cr_assert(arr_eq(cube.edges, solved_cube.edges, 12));
  cr_assert(arr_eq(cube.corners, solved_cube.corners, 8));
  
  make_move(&cube, U, 1, c_trans, e_trans);
  cr_assert(arr_eq(cube.edges, solved_cube.edges, 12));
  cr_assert(arr_eq(cube.corners, solved_cube.corners, 8));
  
  make_move(&cube, D, 1, c_trans, e_trans);
  cr_assert(arr_eq(cube.edges, solved_cube.edges, 12));
  cr_assert(arr_eq(cube.corners, solved_cube.corners, 8));
  
  make_move(&cube, F, 1, c_trans, e_trans);
  cr_assert(arr_eq(cube.edges, solved_cube.edges, 12));
  cr_assert(arr_eq(cube.corners, solved_cube.corners, 8));
  
  make_move(&cube, B, 1, c_trans, e_trans);
  cr_assert(arr_eq(cube.edges, solved_cube.edges, 12));
  cr_assert(arr_eq(cube.corners, solved_cube.corners, 8));
}

Test(cube_moves, single_U_move, .disabled=true){
  uint16_t c_trans[NMOVES][NCORNERCUBIES];
  uint16_t e_trans[NMOVES][NEDGECUBIES];

  initialize_move_tables(c_trans, e_trans);
  gen_move_tables(c_trans, e_trans);

  cube_t solved_cube = cube_create_new_cube();
  cube_t cube = cube_create_new_cube();

  /* A U move changes the eo on UD-axis, so edges on U face will have 0b001 eo-bits.*/
  uint16_t edges_after_U[NEDGES] = {
    (1 << 3) | 0b001, (2 << 3) | 0b001, (3 << 3) | 0b001, (0 << 3) | 0b001,
    4 << 3, 5 << 3, 6 << 3, 7 << 3,
    8 << 3, 9 << 3, 10 << 3, 11 << 3,
  };

  /* A U move changes co on ud*/
  uint16_t corners_after_U[NCORNERS] = {
    1 | 0b01000000, 2 | 0b10000000, 3 | 0b01000000, 0 | 0b10000000,
    4, 5, 6, 7,
  };

  make_move(&cube, U, 0, c_trans, e_trans);

  cr_assert(arr_eq(cube.edges, edges_after_U, NEDGES));
  cr_assert(arr_eq(cube.corners, corners_after_U, NCORNERS));
}

Test(cube_moves, single_D_move){

}

Test(cube_moves, single_R_move){

}

Test(cube_moves, single_L_move){

}


Test(cube_moves, single_F_move){

}

Test(cube_moves, single_B_move){

}