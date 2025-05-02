#include "_solvers.h"
#include "_print_cube.h"
// stats (temp)

struct stats {
  int depth;
  int* solution;
  int* solution_inv;
  int num_sol_found;
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

  stats->num_sol_found = 0;
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
  if (stats->no_nodes_visited == 0){
    fprintf(stderr, "No nodes visited\n");
  } else {
    fprintf(stderr, "\n");
    fprintf(stderr, "Some statistics:\n");
    fprintf(stderr, "max depth: %i\n", stats->depth);
    fprintf(stderr, "sols found: %i\n", stats->num_sol_found);
    fprintf(stderr, "\n");
    fprintf(stderr, "nodes visited: %i\n", stats->no_nodes_visited);
    fprintf(stderr, "nodes pruned: %i (%.2f%%)\n",
      stats->no_nodes_pruned,
      100 * (float)stats->no_nodes_pruned / (float)stats->no_nodes_visited
    );
    fprintf(stderr, "inverse computations: %i (%.2f%%)\n",
      stats->no_inverse_computations,
      100 * (float)stats->no_inverse_computations / (float)stats->no_nodes_visited
    );
    fprintf(stderr, "nisses: %i (%.2f%%)\n",
      stats->no_nisses,
      100 * (float)stats->no_nisses / (float)stats->no_nodes_visited
    );
  }
}


// work in progress 


// private


static inline
bool prune(uint8_t x1, uint8_t x2, uint8_t x3, int rem_moves){
  return ((x1 == rem_moves) && (x1 == x2) && (x2 == x3))
    || (x1 > rem_moves) || (x2 > rem_moves) || (x3 > rem_moves);
}

static bool
TreeSearch(
    cube_t* cube,
    uint8_t* ptable,
    struct c_index_cclass_sym* cclass,
    int remaining_moves,
    struct stats* stats,
    int prev_move,
    int prev_move_inv,
    bool is_inv,
    int max_num_sols,
    bool niss
){
  stats->no_nodes_visited++;

  if (remaining_moves == 0){
    if (cube_state_is_solved(cube)){
      stats->num_sol_found++;
      cube_print_solution_string(stats->solution, stats->solution_inv);
      return stats->num_sol_found == max_num_sols;
    } else {
      return false;
    }
  }

  uint64_t p1 = cube_to_H_index(cube, cclass);
  const uint8_t pval_UD = ptable_read_val(p1, ptable);

  cube_t cube_z_rot = cube_operation_sym_conjugate(*cube, 7);
  uint64_t p2 = cube_to_H_index(&cube_z_rot, cclass);
  const uint8_t pval_LR = ptable_read_val(p2, ptable);

  cube_t cube_x_rot = cube_operation_sym_conjugate(*cube, 9);  
  uint64_t p3 = cube_to_H_index(&cube_x_rot, cclass);
  const uint8_t pval_FB = ptable_read_val(p3, ptable);

  if (prune(pval_UD, pval_LR, pval_FB, remaining_moves)) {
    stats->no_nodes_pruned++;
    return false;
  }

  // we only look up inverse if we did not prune on normal
  cube_t inv = cube_operation_inverse(*cube);
  stats->no_inverse_computations++;

  uint64_t p1_inv = cube_to_H_index(&inv, cclass);
  const uint8_t pval_UD_inv = ptable_read_val(p1_inv, ptable);

  cube_t cube_z_rot_inv = cube_operation_sym_conjugate(inv, 7);
  uint64_t p2_inv = cube_to_H_index(&cube_z_rot_inv, cclass);
  const uint8_t pval_LR_inv = ptable_read_val(p2_inv, ptable);

  cube_t cube_x_rot_inv = cube_operation_sym_conjugate(inv, 9);  
  uint64_t p3_inv = cube_to_H_index(&cube_x_rot_inv, cclass);
  const uint8_t pval_FB_inv = ptable_read_val(p3_inv, ptable);

  if (prune(pval_UD_inv, pval_LR_inv, pval_FB_inv, remaining_moves)){
    stats->no_nodes_pruned++;
    return false;
  }

  // find out if we niss or not!
  bool _niss = false;
  if (niss) {
    int num_axis_to_check_normal = 0;
    int num_axis_to_check_inverse = 0;

    if (pval_UD < remaining_moves) num_axis_to_check_inverse++;
    if (pval_LR < remaining_moves) num_axis_to_check_inverse++;
    if (pval_FB < remaining_moves) num_axis_to_check_inverse++;
    if (pval_UD_inv < remaining_moves) num_axis_to_check_normal++;
    if (pval_LR_inv < remaining_moves) num_axis_to_check_normal++;
    if (pval_FB_inv < remaining_moves) num_axis_to_check_normal++;

    _niss = num_axis_to_check_normal > num_axis_to_check_inverse;
  }

  // niss
  if (_niss) {
    swap_cubes(cube, &inv);
    is_inv = is_inv ? false : true;
    stats->no_nisses++;
  }

  // we extract the move_mask for this node.
  uint32_t mm = move_mask[is_inv ? prev_move_inv : prev_move];

  // nissing magic
  if (remaining_moves == (_niss ? pval_UD : pval_UD_inv)) mm &= ~(0b111111 << 0);
  if (remaining_moves == (_niss ? pval_LR : pval_LR_inv)) mm &= ~(0b111111 << 6);
  if (remaining_moves == (_niss ? pval_FB : pval_FB_inv)) mm &= ~(0b111111 << 12);

  for (int move = 0; move < NMOVES; move++){
      // check if we actually need to do move.
      if (!(mm & (0b1 << move))) continue;

      cube_move_apply_move(cube, move);

      if (!is_inv) stats->solution[20 - remaining_moves] = move;
      else stats->solution_inv[20 - remaining_moves] = move;

      bool found = TreeSearch(
          cube,
          ptable,
          cclass,
          remaining_moves - 1,
          stats,
          !is_inv ? move : prev_move,       // prev_move on normal
          is_inv ? move : prev_move_inv,    // prev_move on inverse
          is_inv,
          max_num_sols,
          niss
      );
      
      cube_move_apply_move(cube, get_inv_move(move));

      if (!is_inv) stats->solution[20 - remaining_moves] = -1;
      else stats->solution_inv[20 - remaining_moves] = -1;

      if (found) return true;
  }

  if (_niss) swap_cubes(cube, &inv);
  return false;
}

void
IDA(
    cube_t cube,
    int MAX_DEPTH,
    uint8_t* ptable,
    struct c_index_cclass_sym* cclass,
    struct stats* stats,
    int max_num_sols,
    bool niss
){
  bool stop_search = false;
  for (int depth = 0; depth <= MAX_DEPTH; depth++){
    init_stats(stats);
    stats->depth = depth;
    printf("Searching depth %i\n", depth);

    stop_search = TreeSearch(
        &cube,
        ptable,
        cclass,
        depth,
        stats,
        18, 18,   // NULLMOVE. TODO: formalise 
        false,
        max_num_sols,
        niss
    );
  
    if (stop_search) return;
  }
}


void
solve_cube(
    cube_t cube,
    int max_depth,
    uint8_t* ptable,
    struct c_index_cclass_sym* cclass,
    int max_num_sols
){
  // we collect some stats along the way.
  struct stats* stats = malloc(sizeof(struct stats));
  init_stats(stats);

  // actually search.
  IDA(
      cube,
      max_depth,
      ptable,
      cclass,
      stats,
      max_num_sols,
      false
  );

  print_stats(stats);

  free(stats->solution);
  free(stats->solution_inv);
  free(stats);
}


/* public */

bool
cube_solvers_solve_cube(cube_t cube, int number_of_solutions, int* banned_moves, int num_banned_moves){
  fprintf(stderr, "This solver is WIP\n");
  // cube_print_cube(&cube);

  // todo: handle these precomputations
  cube_tables_generate();
  precompute_combinatorials();    // need to precompute these at the moment.

  // todo: maybe split the solver into one that solves optimally
  // for one solution and one that is used for alg genning?
  for (int i = 0; i < num_banned_moves; i++){
    movemask_remove_move(banned_moves[i]);
  }

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

  const uint64_t size_cclass = sizeof(struct c_index_cclass_sym) * NCCU * NCO;

  fprintf(stderr, "Allocating %lu B for cclass table\n", size_cclass);
  struct c_index_cclass_sym* cclass = malloc(size_cclass);

  fprintf(stderr, "WIP: Loading cclass table\n");

  if (!load_cclasstable("cclass.dat", cclass, size_cclass)) {
    fprintf(stderr, "Could not load cclass table from %s\n", "cclass.dat");
    free(ptable);
    free(cclass);
    return false;
  }

  // set a max limit.
  int max_depth = 18;
  fprintf(stderr, "\nMax depth set to: %i\n", max_depth);

  solve_cube(cube, max_depth, ptable, cclass, number_of_solutions);

  free(ptable);
  free(cclass);
  return true;
}
