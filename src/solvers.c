#include "_solvers.h"

// stats (temp)

struct stats {
  int depth;
  int* solution;
  int* solution_inv;
  unsigned int no_nodes_visited;
  unsigned int no_nodes_pruned;
  unsigned int no_inverse_computations;
  int no_equal_pvals_normal;
  int no_equal_pvals_inverse;
};


static void
init_stats(struct stats* stats){
  stats->depth = 0;
  stats->no_nodes_visited = 0;
  stats->no_nodes_pruned = 0; 
  stats->no_inverse_computations = 0;
  stats->no_equal_pvals_normal = 0;
  stats->no_equal_pvals_inverse = 0;

  stats->solution = malloc(sizeof(int)*20);
  for (int i = 0; i < 20; i++){
    stats->solution[i] = -1;
  }

  stats->solution_inv = malloc(sizeof(int)*20);
  for (int i = 0; i < 20; i++){
    stats->solution_inv[i] = -1;
  }
}

static void
print_stats(struct stats* stats){
  printf("\n");
  printf("Some statistics:\n");
  printf("Depth: %i\n", stats->depth);
  printf("\n");
  printf("No equal pvals normal: %i\n", stats->no_equal_pvals_normal);
  printf("No equal pvals inverse: %i\n", stats->no_equal_pvals_inverse);
  printf("\n");
  printf("No nodes visited: %i\n", stats->no_nodes_visited);
  printf("No nodes pruned: %i\n", stats->no_nodes_pruned);
  printf("No nodes pruned / visited: %f\n", (float)stats->no_nodes_pruned / (float)stats->no_nodes_visited);
  printf("No inverse computations: %i\n", stats->no_inverse_computations);
  printf("No inverse computations / visited: %f\n", (float)stats->no_inverse_computations / (float)stats->no_nodes_visited);
}


// work in progress 



// private

static void
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

static inline void
swap_cubes(cube_t* c, cube_t* t){
 cube_t aux = *t;
 *t = *c;
 *c = aux;
}

static inline bool
we_can_skip_move(int move, int prev_move){
  return move/6 == prev_move/6 && move/3 >= prev_move/3;
}

static inline
bool prune(uint16_t x1, uint16_t x2, uint16_t x3, int depth, struct stats* stats){
  if ( (x1 == depth) && (x1 == x2) && (x2 == x3)){
    stats->no_equal_pvals_inverse++;
    stats->no_nodes_pruned++;
    return true;
  }
  
  if (
    (x1 != 21 && x1 > depth) ||
    (x2 != 21 && x2 > depth) ||
    (x3 != 21 && x3 > depth)
  ) {
    stats->no_nodes_pruned++;
    return true;
  }
  return false;
}


static bool
TreeSearch(
    cube_t* cube,
    int* moveset,
    int moveset_length,
    bool (*cube_solved)(cube_t* cube),
    unsigned int (*cube_to_index)(cube_t* cube),
    uint16_t* ptable,
    int depth,
    struct stats* stats,
    int prev_move,
    int prev_move_inv,
    bool is_inv
){
  stats->no_nodes_visited++;
  if (depth == 0) return cube_solved(cube);  // we reached the end...

  /*
   * z = (y)^3(y z)^1
   * z: (x1, x2, x3, x4) = (3, 1, 0, 0)
   * 3*1 + 1*4 + 0*12 + 0*24 = 7
   *
   * x' = y (y z)^2
   * x': (x1, x2, x3, x4) = (1, 2, 0, 0)
   * 1*1 + 2*4 + 0*12 + 0*24 = 9
   */
  
/*
 *
 * TODO: 
 * investigate nissing
 * improve ptable memory management
 * investigate ep / cp pruning
 *
 * */

  unsigned int p1 = cube_to_index(cube);
  cube_t cube_z_rot = cube_operation_sym_conjugate(*cube, 7);
  unsigned int p2 = cube_to_index(&cube_z_rot);
  cube_t cube_x_rot = cube_operation_sym_conjugate(*cube, 9);  
  unsigned int p3 = cube_to_index(&cube_x_rot);

  const uint16_t x1 = ptable[p1];
  const uint16_t x2 = ptable[p2];
  const uint16_t x3 = ptable[p3];

  if (prune(x1, x2, x3, depth, stats)) return false;

  // we only look up inverse if we have gotten here
  cube_t inv = cube_operation_inverse(*cube);
  stats->no_inverse_computations++;

  unsigned int p1_inv = cube_to_index(&inv);
  cube_t cube_z_rot_inv = cube_operation_sym_conjugate(inv, 7);
  unsigned int p2_inv = cube_to_index(&cube_z_rot_inv);
  cube_t cube_x_rot_inv = cube_operation_sym_conjugate(inv, 9);  
  unsigned int p3_inv = cube_to_index(&cube_x_rot_inv);

  const uint16_t x1_inv = ptable[p1_inv];
  const uint16_t x2_inv = ptable[p2_inv];
  const uint16_t x3_inv = ptable[p3_inv];

  if (prune(x1_inv, x2_inv, x3_inv, depth, stats)) return false;
/*
  fprintf(stderr, "Now at depth %i\n", depth);
  fprintf(stderr, "\tnormal: %i, %i, %i\n", x1, x2, x3);
  fprintf(stderr, "\tinverse: %i, %i, %i\n", x1_inv, x2_inv, x3_inv);
*/ 
  // find out if we niss or not!
  bool niss = (depth % 5 == 0);

  if (niss) {
    swap_cubes(cube, &inv);
    is_inv = is_inv ? false : true;
  }


  for (int i = 0; i < moveset_length; i++){
      int move = moveset[i];
      // check if we actually need to do move.
      if (!we_can_skip_move(move, is_inv ? prev_move_inv : prev_move )){
        cube_move_apply_move(cube, move);

        bool found = TreeSearch(
            cube,
            moveset,
            moveset_length,
            cube_solved,
            cube_to_index,
            ptable,
            depth - 1,
            stats,
            !is_inv ? move : prev_move,       // prev_move on normal
            is_inv ? move : prev_move_inv,    // prev_move on inverse
            is_inv
        );

        apply_inverse_move(cube, move);

        if (found){
          if (niss) {
            swap_cubes(cube, &inv);
          }

          if (!is_inv) stats->solution[depth - 1] = move;
          else stats->solution_inv[depth - 1] = move;

          return found;
        }
      }
  }

  if (niss) {
    swap_cubes(cube, &inv);
  }
  return false;
}

bool
IDA(
    cube_t cube,
    int* moveset,
    int moveset_length,
    bool (*cube_solved)(cube_t* cube),
    int MAX_DEPTH,
    struct stats* stats
)
{
  // prepare prune table and index function.
  unsigned int (*cube_to_index)(cube_t*) = cube_to_L_index;
  char* filename = "L.dat";

  // load pruning table
  fprintf(stderr, "Allocating %lu B for pruning table\n", sizeof(uint16_t)*SIZEL / (8));
  uint16_t* ptable = malloc(sizeof(uint16_t)*SIZEL);

  if (!load_ptable(filename, ptable)){
    fprintf(stderr, "Could not load pruning table from %s\n", filename);
    free(ptable);
    return false; 
  };

  bool found = false;
  for (int depth = 0; depth <= MAX_DEPTH; depth++){
    // fprintf(stderr, "Initializing stats for depth %i\n", depth);
    init_stats(stats);
    stats->depth = depth;
    fprintf(stderr, "Searching depth %i\n", depth);
    found = TreeSearch(
        &cube,
        moveset,
        moveset_length,
        cube_solved,
        cube_to_index,
        ptable,
        depth,
        stats,
        18,   // NULLMOVE. TODO: formalise
        18,   // NULLMOVE
        false
    );
  
    if (found) {
        free(ptable);
        return found;
      }
    }
    
  free(ptable);
  return false;
}


char *move_notation[] = {
  "U", "U2", "U'",
  "D", "D2", "D'",
  "L", "L2", "L'",
  "R", "R2", "R'",
  "F", "F2", "F'",
  "B", "B2", "B'",
};


/* public */

bool
cube_solvers_solve_cube(cube_t cube, int* solution, int solution_length){
  fprintf(stderr, "WIP\n");
  cube_print_cube(&cube);

  // todo: handle these precomputations
  cube_tables_generate();
  precompute_combinatorials();    // need to precompute these at the moment.

  const int num_moves_in_moveset = 18;
  int moveset[] = {
    R1, R2, R3, U1, U2, U3, F1, F2, F3, B1, B2, B3, L1, L2, L3, D1, D2, D3
  };

  // we collect some stats along the way.
  struct stats* stats = malloc(sizeof(struct stats));

  // set a max limit.
  int max_depth = 15;
  printf("Max depth set to: %i\n", max_depth);

  // actually search.
  bool found_sol = IDA(
      cube,
      moveset,
      num_moves_in_moveset,
      &cube_state_is_solved,
      max_depth,
      stats
  );

  print_stats(stats);

  if (found_sol){
    //int sol_len = 0;
    printf("normal: ");
    for (int i = 19; i >= 0; i--){
      if (stats->solution[i] != -1){
        printf("%s ", move_notation[stats->solution[i]]);
        //sol_len++;
      }
    }
    printf("\n");
    printf("inverse: (");
    for (int i = 19; i >= 0; i--){
      if (stats->solution_inv[i] != -1){
        printf("%s ", move_notation[stats->solution_inv[i]]);
        //sol_len++;
      }
    }
    printf(")\n");
    // printf("\nThe solution length is %i\n", sol_len);
  } else {
    printf("Did not find a solution!\n");
  }

  free(stats->solution);
  free(stats->solution_inv);
  free(stats);

  return found_sol;
}
