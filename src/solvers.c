#include "_solvers.h"

void
apply_inverse_move(cube_t* cube, int move){
    if (move % 3 == 0){
      cube_move_apply_move(cube, move + 2);
    }
    else if (move % 3 == 2) {
      cube_move_apply_move(cube, move - 2);
    }
    else {
      cube_move_apply_move(cube, move);
    }
}

bool
DLS(cube_t* cube, int* moveset, size_t moveset_length, bool (*cube_solved)(cube_t* cube), int depth, int* solution){
  if (depth == 0){
    return cube_solved(cube);
  }
  
  for (size_t i = 0; i < moveset_length; i++){
    cube_move_apply_move(cube, moveset[i]);
    bool found = DLS(cube, moveset, moveset_length, cube_solved, depth - 1, solution);
    apply_inverse_move(cube, moveset[i]);

    if (found){
      for (int j = 0; j < depth; j++){
        if (solution[j] == -1){
          solution[j] = moveset[i];
        }
      }
      return found;
    }
  }

  return false;
}

bool
IDDFS(cube_t cube, int* moveset, size_t moveset_length, bool (*cube_solved)(cube_t* cube), int MAX_DEPTH, int* solution)
{
  // prepare solution array
  for (int i = 0; i < 20; i++){
    solution[i] = -1;
  }
  bool found = false;
  for (int depth = 0; depth <= MAX_DEPTH; depth++){
    fprintf(stderr, "Searching depth %i\n", depth);
    found = DLS(&cube, moveset, moveset_length, cube_solved, depth, solution);

    if (found) {
      return found;
    }
  }

  return false;
}

