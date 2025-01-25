#include "_tables.h"

uint16_t prune_table_cp[FACT8];

/* Generic implementation of Depth Limited Search
`cube_to_index` is used to convert a cube to the corresponding
index in the table `ptable`*/
static void
DLS(cube_t* cube, int depth, int prev_move, int num_moves_done, int (*cube_to_index)(cube_t* cube), uint16_t* ptable);

int
get_inv_move(int move){
    if (move % 3 == 0) return move + 2;
    else if (move % 3 == 2) return move - 2;
    return move;
}

/* -------------------- cp ---------------------- */

int
cube_to_cp_index(cube_t* cube){
    int perm[NCORNERS];
    for (int i = 0; i < NCORNERS; i++){
        perm[i] = extract_corner_perm(cube->corners[i]);
    } 
    return perm_to_fact(perm, NCORNERS);
}

void
gen_ptable_cp()
{
    cube_t cube = cube_create_new_cube();

    prune_table_cp[0] = 0; // solved is solved
    for (int i = 1; i < FACT8; i++){
        prune_table_cp[i] = 20; // a default admissible heuristic.
    }

    for (int depth = 0; depth <= 7; depth++){
        DLS(&cube, depth, 18, 1, cube_to_cp_index, prune_table_cp);
    }
}

static void
DLS(cube_t* cube, int depth, int prev_move, int num_moves_done, int (*cube_to_index)(cube_t* cube), uint16_t* ptable){
    if (depth == 0) return;
    int p;
  
    for (int m = 0; m < NMOVES; m++){
        // do not check redundant sequences, ex.: R R, R L R.
        if (!(m/6 == prev_move/6 && m/3 >= prev_move/3)){
            cube_move_apply_move(cube, m);

            p = cube_to_index(cube);

            if (ptable[p] == 20 || ptable[p] == num_moves_done){
                ptable[p] = (uint16_t)num_moves_done;
                DLS(cube, depth - 1, m, num_moves_done + 1, cube_to_index, ptable);
            }
            cube_move_apply_move(cube, get_inv_move(m));
        }
    }
}
