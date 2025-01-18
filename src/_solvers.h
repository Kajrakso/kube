#ifndef __SOLVERS_H_
#define __SOLVERS_H_

#include "_cube.h"
#include "_cube_state.h"
#include "../include/solvers/solvers.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

/*
Iterative deepening depth first search.

Finds a shortest solution using IDDFS.
OBS: This is very slow.
To be used only when the cube state is close to the target
given by the function cube_solved.

Example usage:

```C
const int num_moves_in_moveset = 9;
int moveset[] = { R1, R2, R3, U1, U2, U3, F1, F2, F3};
int solution[20] = {0};
int max_depth = 5;

bool found_sol = IDDFS(
    cube, moveset, num_moves_in_moveset, &cube_state_is_solved, max_depth, solution
);
```

*/
bool
IDDFS(cube_t cube, int* moveset, size_t moveset_length, bool (*cube_solved)(cube_t* cube), int MAX_DEPTH, int* solution);

#endif /* __SOLVERS_H_ */
