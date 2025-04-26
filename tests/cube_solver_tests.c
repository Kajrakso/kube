#include "../include/libcube.h"
#include "../src/_cube.h"
#include "../src/_index.h"
#include "../src/_solvers.h"
#include "../src/_cube_state.h"
#include "criterion/criterion.h"
#include "criterion/redirect.h"
#include "../src/utils.h"
#include <stdint.h>
#include <stdio.h>

// suites

TestSuite(solver, .disabled=false);

// tests

Test(solver, some_simple_scrambles){
  /* ------------------ preparation */
  cube_tables_generate();
  precompute_combinatorials();    // need to precompute these at the moment.

  // prepare prune table and index function.
  char* filename = "H.dat";

  // load pruning table
  uint8_t* ptable = malloc(sizeof(uint8_t) * SIZE_PTABLE_H);

  if (!load_ptable("H.dat", ptable, sizeof(uint8_t) * SIZE_PTABLE_H)) {
    fprintf(stderr, "Could not load pruning table from %s\n", filename);
    free(ptable);
    cr_assert(false);
  }

  const uint64_t size_cclass =
    sizeof(struct c_index_cclass_sym) * NCCU * NCO;

  struct c_index_cclass_sym* cclass = malloc(size_cclass);

  if (!load_cclasstable("cclass.dat", cclass, size_cclass)) {
    fprintf(stderr, "Could not load cclass table from %s\n", "cclass.dat");
    free(ptable);
    free(cclass);
    cr_assert(false);
  }

  // set a max limit.
  int max_depth = 4;

  // Allocate some space for the solution
  const int max_sol_size = 20;
  int* solution = malloc(sizeof(int) * max_sol_size);
  for (int i = 0; i < 20; i++) solution[i] = -1;

  int* optimal_solution = malloc(sizeof(int) * max_sol_size);
  /* ------------------ end preparation */



  // run some tests
  // TEST SCRAMBLE 1
  cube_t cube;
  int optimal_solution_length;
  cube = cube_create_new_cube();
  cube_move_apply_move_string(&cube, "R U L");
  optimal_solution_length = 3;

  for (int i = 0; i < 20; i++) optimal_solution[i] = -1;
  optimal_solution[0] = L3;
  optimal_solution[1] = U3;
  optimal_solution[2] = R3;

  solve_cube(cube, max_depth, ptable, cclass, solution);

  for (int i = 0; i < optimal_solution_length; i++){
    if (solution[i] != -1) cube_move_apply_move(&cube, solution[i]);
  }
  
  cr_assert(cube_state_is_solved(&cube));

  // TEST SCRAMBLE 2
  cube = cube_create_new_cube();
  cube_move_apply_move_string(&cube, "R' U' L'");
  optimal_solution_length = 3;

  solve_cube(cube, max_depth, ptable, cclass, solution);

  for (int i = 0; i < optimal_solution_length; i++){
    if (solution[i] != -1) cube_move_apply_move(&cube, solution[i]);
  }
  
  cr_assert(cube_state_is_solved(&cube));

  // TEST SCRAMBLE 3
  cube = cube_create_new_cube();
  cube_move_apply_move_string(&cube, "F U2 B'");
  optimal_solution_length = 3;

  solve_cube(cube, max_depth, ptable, cclass, solution);

  for (int i = 0; i < optimal_solution_length; i++){
    if (solution[i] != -1) cube_move_apply_move(&cube, solution[i]);
  }
  
  cr_assert(cube_state_is_solved(&cube));

  // TEST SCRAMBLE 4
  cube = cube_create_new_cube();
  cube_move_apply_move_string(&cube, "R U L D");
  optimal_solution_length = 4;

  solve_cube(cube, max_depth, ptable, cclass, solution);
  
  for (int i = 0; i < optimal_solution_length; i++){
    if (solution[i] != -1) cube_move_apply_move(&cube, solution[i]);
  }
  
  cr_assert(cube_state_is_solved(&cube));

  // TEST SCRAMBLE 5
  cube = cube_create_new_cube();
  cube_move_apply_move_string(&cube, "R2 U R2 U");
  optimal_solution_length = 4;

  solve_cube(cube, max_depth, ptable, cclass, solution);

  for (int i = 0; i < optimal_solution_length; i++){
    if (solution[i] != -1) cube_move_apply_move(&cube, solution[i]);
  }
  
  cr_assert(cube_state_is_solved(&cube));

  // TEST SCRAMBLE 6
  cube = cube_create_new_cube();
  cube_move_apply_move_string(&cube, "U2 L2 D2 F");
  optimal_solution_length = 4;

  solve_cube(cube, max_depth, ptable, cclass, solution);
  
  for (int i = 0; i < optimal_solution_length; i++){
    if (solution[i] != -1) cube_move_apply_move(&cube, solution[i]);
  }
  
  cr_assert(cube_state_is_solved(&cube));

  free(solution);
  free(optimal_solution);
  free(ptable);
  free(cclass);
}


Test(solver, some_mid_scrambles){
  /* ------------------ preparation */
  cube_tables_generate();
  precompute_combinatorials();    // need to precompute these at the moment.

  // prepare prune table and index function.
  char* filename = "H.dat";

  // load pruning table
  uint8_t* ptable = malloc(sizeof(uint8_t) * SIZE_PTABLE_H);

  if (!load_ptable("H.dat", ptable, sizeof(uint8_t) * SIZE_PTABLE_H)) {
    fprintf(stderr, "Could not load pruning table from %s\n", filename);
    free(ptable);
    cr_assert(false);
  }

  const uint64_t size_cclass =
    sizeof(struct c_index_cclass_sym) * NCCU * NCO;

  struct c_index_cclass_sym* cclass = malloc(size_cclass);

  if (!load_cclasstable("cclass.dat", cclass, size_cclass)) {
    fprintf(stderr, "Could not load cclass table from %s\n", "cclass.dat");
    free(ptable);
    free(cclass);
    cr_assert(false);
  }

  // set a max limit.
  int max_depth = 15;

  // Allocate some space for the solution
  const int max_sol_size = 20;
  int* solution = malloc(sizeof(int) * max_sol_size);
  for (int i = 0; i < 20; i++) solution[i] = -1;

  int* optimal_solution = malloc(sizeof(int) * max_sol_size);
  /* ------------------ end preparation */


  // run some tests
  // TEST SCRAMBLE 1
  cube_t cube;
  cube = cube_create_new_cube();
  cube_move_apply_move_string(&cube, "F' U' D B' U' R' L' F D' L F2 U' F2");
  const int optimal_solution_length = 13;

  solve_cube(cube, max_depth, ptable, cclass, solution);
  
  for (int i = 0; i < optimal_solution_length; i++){
    if (solution[i] != -1) cube_move_apply_move(&cube, solution[i]);
  }

  cr_assert(cube_state_is_solved(&cube));
  
  free(solution);
  free(ptable);
  free(cclass);
}



