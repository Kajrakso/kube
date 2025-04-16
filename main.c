#include "include/libcube.h"
#include "string.h"
#include <stdio.h>

// Example of how to use libcube
int main(){
  // Per now we need to do this manually first
  cube_tables_generate();

  // Set a scramble sequence. This one is 13 moves optimal.
  char* scr = "D' B' D F B' U F2 U2 B R2 U2 R2 F D2 R2 L2";
  
  // Create a new cube
  cube_t cube = cube_create_new_cube();

  // Scramble the cube
  printf("\nScramble: %s\n", scr);
  cube_move_apply_move_string(&cube, scr);

  // Allocate some space for the solution
  const int max_sol_size = 20;
  int* solution = malloc(sizeof(int) * max_sol_size);
  for (int i = 0; i < 20; i++){
    solution[i] = -1;
  }

  bool solved = cube_solvers_solve_cube(cube, solution, max_sol_size);

  if (solved) {
    cube_print_solution_string(solution, max_sol_size);
  }
  else {
    printf("Was not able to solve the cube :(\n");
  }

  free(solution);
  return 0;
}
