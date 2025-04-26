#include "_solvers.h"

// stats (temp)

struct stats {
  int depth;
  int* solution;
  int* solution_inv;
  unsigned int no_nisses;
  unsigned int no_nodes_visited;
  unsigned int no_nodes_pruned;
  unsigned int no_inverse_computations;
  int no_equal_pvals_normal;
  int no_equal_pvals_inverse;
};

void
init_stats(struct stats* stats){
  stats->depth = 0;
  stats->no_nodes_visited = 0;
  stats->no_nodes_pruned = 0; 
  stats->no_nisses = 0;
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
  printf("No nisses: %i\n", stats->no_nisses);
  printf("\n");
  printf("No nodes visited: %i\n", stats->no_nodes_visited);
  printf("No nodes pruned: %i\n", stats->no_nodes_pruned);
  printf("No nodes pruned / visited: %f\n", (float)stats->no_nodes_pruned / (float)stats->no_nodes_visited);
  printf("No inverse computations: %i\n", stats->no_inverse_computations);
  printf("No inverse computations / visited: %f\n", (float)stats->no_inverse_computations / (float)stats->no_nodes_visited);
}


// work in progress 


// private


static inline
bool prune(uint8_t x1, uint8_t x2, uint8_t x3, int depth){
  if (x1 != 15 && (x1 == depth) && (x1 == x2) && (x2 == x3)){
    return true;
  }
  
  if ((x1 != 15 && x1 > depth) || (x2 != 15 && x2 > depth) || (x3 != 15 && x3 > depth)) {
    return true;
  }
  return false;
}

static bool
TreeSearch(
    cube_t* cube,
    uint8_t* ptable,
    struct c_index_cclass_sym* cclass,
    int depth,
    struct stats* stats,
    int prev_move,
    int prev_move_inv,
    bool is_inv
){
  stats->no_nodes_visited++;
  if (depth == 0){
    return cube_state_is_solved(cube);  // we reached the end...
  }

  uint64_t p1 = cube_to_H_index(cube, cclass);
  cube_t cube_z_rot = cube_operation_sym_conjugate(*cube, 7);
  uint64_t p2 = cube_to_H_index(&cube_z_rot, cclass);
  cube_t cube_x_rot = cube_operation_sym_conjugate(*cube, 9);  
  uint64_t p3 = cube_to_H_index(&cube_x_rot, cclass);

  const uint8_t x1 = ptable_read_val(p1, ptable);
  const uint8_t x2 = ptable_read_val(p2, ptable);
  const uint8_t x3 = ptable_read_val(p3, ptable);

  if (prune(x1, x2, x3, depth)) {
    stats->no_nodes_pruned++;
    return false;
  }

  // we only look up inverse if we did not prune on normal
  cube_t inv = cube_operation_inverse(*cube);
  stats->no_inverse_computations++;

  uint64_t p1_inv = cube_to_H_index(&inv, cclass);
  cube_t cube_z_rot_inv = cube_operation_sym_conjugate(inv, 7);
  uint64_t p2_inv = cube_to_H_index(&cube_z_rot_inv, cclass);
  cube_t cube_x_rot_inv = cube_operation_sym_conjugate(inv, 9);  
  uint64_t p3_inv = cube_to_H_index(&cube_x_rot_inv, cclass);

  const uint8_t x1_inv = ptable_read_val(p1_inv, ptable);
  const uint8_t x2_inv = ptable_read_val(p2_inv, ptable);
  const uint8_t x3_inv = ptable_read_val(p3_inv, ptable);

  if (prune(x1_inv, x2_inv, x3_inv, depth)){
    stats->no_nodes_pruned++;
    return false;
  }

  // find out if we niss or not!
  // bool niss = (depth % 5 == 0);
  
  // bool niss = (x1_inv + x2_inv + x3_inv < x1 + x2 + x3);
  // bool niss = false;
  //
  // if (niss) {
  //   swap_cubes(cube, &inv);
  //   is_inv = is_inv ? false : true;
  //   stats->no_nisses++;
  // }

  for (int move = 0; move < NMOVES; move++){
      // check if we actually need to do move.
      // if (!niss){
        if (x1_inv == depth && move / 6 == 0) continue;
        if (x2_inv == depth && move / 6 == 1) continue;
        if (x3_inv == depth && move / 6 == 2) continue;
      // }
      // else {
      //   if (x1 == depth && move / 6 == 0) continue;
      //   if (x2 == depth && move / 6 == 1) continue;
      //   if (x3 == depth && move / 6 == 2) continue;
      // }
      
      if (!(move_mask[is_inv ? prev_move_inv : prev_move] & (0b1 << move))) continue;

      // cube_t c = *cube;
      cube_move_apply_move(cube, move);
      bool found = TreeSearch(
          cube,
          ptable,
          cclass,
          depth - 1,
          stats,
          !is_inv ? move : prev_move,       // prev_move on normal
          is_inv ? move : prev_move_inv,    // prev_move on inverse
          is_inv
      );
      
      cube_move_apply_move(cube, get_inv_move(move));

      if (found){
        // if (niss) {
        //   swap_cubes(cube, &inv);
        // }

        if (!is_inv) stats->solution[depth - 1] = move;
        else stats->solution_inv[depth - 1] = move;

        return found;
      }
  }

  // if (niss) {
  //   swap_cubes(cube, &inv);
  // }
  return false;
}

bool
IDA(
    cube_t cube,
    int MAX_DEPTH,
    uint8_t* ptable,
    struct c_index_cclass_sym* cclass,
    struct stats* stats
){
  bool found = false;
  for (int depth = 0; depth <= MAX_DEPTH; depth++){
    init_stats(stats);
    stats->depth = depth;
    fprintf(stderr, "Searching depth %i\n", depth);
    found = TreeSearch(
        &cube,
        ptable,
        cclass,
        depth,
        stats,
        18,   // NULLMOVE. TODO: formalise
        18,   // NULLMOVE
        false
    );
  
    if (found) {
        return found;
      }
    }
    
  return false;
}


bool
solve_cube(
    cube_t cube,
    int max_depth,
    uint8_t* ptable,
    struct c_index_cclass_sym* cclass,
    int* solution
){
  // we collect some stats along the way.
  struct stats* stats = malloc(sizeof(struct stats));
  init_stats(stats);

  // actually search.
  bool found_sol = IDA(
      cube,
      max_depth,
      ptable,
      cclass,
      stats
  );

  // combine the solutions on normal and inverse to a final solution
  if (found_sol){
    int i = 0, j = 0;
    for (int m = 0; m < 20; m++){
      if (0 <= stats->solution[19 - m] && stats->solution[19 - m] < 18){  
        solution[i] = stats->solution[19 - m];
        i++;
      }
    }

    for (int m = 0; m < 20; m++){
      if (0 <= stats->solution_inv[m] && stats->solution_inv[m] < 18){  
        solution[i + j] = get_inv_move(stats->solution_inv[m]);
        j++;
      }
    }
  }

  print_stats(stats);

  free(stats->solution);
  free(stats->solution_inv);
  free(stats);

  return found_sol;
}


/* public */

bool
cube_solvers_solve_cube(cube_t cube, int* solution, int solution_length){
  fprintf(stderr, "This solver is WIP\n");
  cube_print_cube(&cube);

  // todo: handle these precomputations
  cube_tables_generate();
  precompute_combinatorials();    // need to precompute these at the moment.

  // prepare prune table
  char* filename = "H.dat";

  // load pruning table
  fprintf(stderr, "Allocating %llu B for pruning table\n", sizeof(uint8_t) * SIZE_PTABLE_H);

  uint8_t* ptable = malloc(sizeof(uint8_t) * SIZE_PTABLE_H);

  if (!load_ptable("H.dat", ptable, sizeof(uint8_t) * SIZE_PTABLE_H)) {
    fprintf(stderr, "Could not load pruning table from %s\n", filename);
    free(ptable);
    return false;
  }

  const uint64_t size_cclass =
    sizeof(struct c_index_cclass_sym) * NCCU * NCO;
  fprintf(stderr, "Allocating %lu B for cclass table\n", size_cclass);

  struct c_index_cclass_sym* cclass = malloc(size_cclass);

  fprintf(stderr, "WIP: Loading cclass table\n");
  ;

  if (!load_cclasstable("cclass.dat", cclass, size_cclass)) {
    fprintf(stderr, "Could not load cclass table from %s\n", "cclass.dat");
    free(ptable);
    free(cclass);
    return false;
  }

  // set a max limit.
  int max_depth = 18;
  printf("Max depth set to: %i\n", max_depth);

  bool found_sol = solve_cube(cube, max_depth, ptable, cclass, solution);

  free(ptable);
  free(cclass);

  return found_sol;
}
