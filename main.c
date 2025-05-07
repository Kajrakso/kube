#include "include/libcube.h"
#include <stdio.h>

// Example of how to use libcube
int main(){
  // Per now we need to do this manually first
  cube_tables_generate();
  cube_tables_load();

  // Set a scramble sequence.
  char* scr = "F' L2 F D' B' U2 B' U' B2 D"; // LTCT D[G]

  // Create a new cube
  cube_t cube = cube_create_new_cube();

  // Scramble the cube
  printf("Scramble: %s\n\n", scr);
  cube_move_apply_move_string(&cube, scr);

  // ban some moves from the solution:
  int num_banned = 0;
  int banned[] = {B1, L1, D1};    // RUF-gen

  // set number of solutions you want
  int num_sols = 10;

  bool solved = cube_solvers_solve_cube(cube, num_sols, banned, num_banned);

  if (!solved) printf("Was not able to solve the cube :(\n");

  cube_tables_free();
  return 0;
}
