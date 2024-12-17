#include "print_cube.h"
#include <stdio.h>
#include <stdlib.h>

void print_cube(cube_t* cube){
  printf("edges:\n");
  for (int i = 0; i < 12; i++){
    uint16_t e = cube->edges[i];
    printf("[%i %b], ", e>>3, e&0b111);
  }
  printf("\ncorners:\n");
  for (int i = 0; i < 8; i++){
    uint16_t c = cube->corners[i];
    printf("[%i %b], ", c&0b111, c>>3);
  }
  printf("\n");
}


int main(int argc, char** argv){
  cube_t cube = cube_create_new_cube();
  print_cube(&cube);

  uint16_t c_trans[NMOVES][NCORNERCUBIES];
  uint16_t e_trans[NMOVES][NEDGECUBIES];

  initialize_move_tables(c_trans, e_trans);
  gen_move_tables(c_trans, e_trans);

  make_move(&cube, R, 0, c_trans, e_trans);
  printf("\n");
  print_cube(&cube);

  make_move(&cube, L, 1, c_trans, e_trans);
  printf("\n");
  print_cube(&cube);

  make_move(&cube, U, 2, c_trans, e_trans);
  printf("\n");
  print_cube(&cube);

  make_move(&cube, R, 0, c_trans, e_trans);
  printf("\n");
  print_cube(&cube);
}
