#include "_tables.h"
#include "_index.h"
#include "stdio.h"

// testing
int ece_mtable[495][NMOVES];
int coud_mtable[2187][NMOVES];
int eofb_mtable[2048][NMOVES];

/* Generic implementation of Depth Limited Search
`cube_to_index` is used to convert a cube to the corresponding
index in the table `ptable`*/
/*
static void
DLS(cube_t* cube, int depth, int prev_move, int num_moves_done, unsigned int (*cube_to_index)(cube_t* cube), uint16_t* ptable);
*/
void gen_eofb_mtable();
void gen_coud_mtable();
void gen_ece_mtable();

static void
DLS_L(
    int ece,
    int eofb,
    int coud,
    unsigned int p,
    int depth,
    int prev_move,
    int num_moves_done,
    uint16_t* ptable
);

static inline int
get_inv_move(int move){
    if (move % 3 == 0) return move + 2;
    else if (move % 3 == 2) return move - 2;
    return move;
}

/* -------------------- H ---------------------- */

/*
void
gen_ptable_H()
{
    cube_t cube = cube_create_new_cube();

    uint16_t* ptable_H = malloc(sizeof(uint16_t) * SIZEL);
    char* filename = "H.dat";

    printf("Initializing ptable for group H\n");
    for (size_t i = 1; i < SIZEL; i++){
        ptable_H[i] = 0; // a default admissible heuristic.
    }

    fprintf(stderr, "Start to gen ptable for group H\n");
    for (int depth = 1; depth <= 10; depth++){
        fprintf(stderr, "Searching at depth %i\n", depth);
        DLS(&cube, depth, 18, 1, cube_to_H_index, ptable_H);
    }
    
    fprintf(stderr, "writing ptable for group H to disk. file: %s.\n", filename);
    
    FILE *file = fopen(filename, "wb");
    if (!file)
    {
        free(ptable_H);
        return;
    }

    // Save the array
    fwrite(ptable_H, sizeof(uint16_t) * SIZEL, 1, file);

    fclose(file);
    free(ptable_H);
}
*/

//! Note: this is a big table. there might be performance issues.
/*
void
gen_ptable_K()
{
    cube_t cube = cube_create_new_cube();

    uint16_t* ptable_K = malloc(sizeof(uint16_t) * SIZEL);

    char* filename = "K.dat";

    printf("Initializing ptable for group K\n");
    for (size_t i = 1; i < SIZEL; i++){
        // ptable_K[i] = 0; // a default admissible heuristic.
        ptable_K[i] = 21; // a dummy val
    }

    fprintf(stderr, "Start to gen ptable for group K\n");
    ptable_K[cube_to_K_index(&cube)] = 0;
    for (int depth = 1; depth <= 9; depth++){
        fprintf(stderr, "Searching at depth %i\n", depth);
        DLS(&cube, depth, 18, 1, cube_to_K_index, ptable_K);
    }
    
    fprintf(stderr, "writing ptable for group K to disk. file: %s.\n", filename);
    
    FILE *file = fopen(filename, "wb");
    if (!file)
    {
        free(ptable_K);
        return;
    }

    // Save the array
    fwrite(ptable_K, sizeof(uint16_t) * SIZEL, 1, file);

    fclose(file);
    free(ptable_K);
}
*/

//! Note: this is a big table. there might be performance issues.
void
gen_ptable_L()
{
    cube_t cube = cube_create_new_cube();
    precompute_combinatorials();


    fprintf(stderr, "Generating move tables\n");
    fprintf(stderr, "gen_ece_mtable\n");
    gen_ece_mtable();
    fprintf(stderr, "gen_coud_mtable\n");
    gen_coud_mtable();
    fprintf(stderr, "gen_eofb_mtable\n");
    gen_eofb_mtable();

    uint16_t* ptable_L = malloc(sizeof(uint16_t) * SIZEL);

    printf("Initializing ptable for group L\n");
    for (size_t i = 1; i < SIZEL; i++){
        // ptable_L[i] = 0; // a default admissible heuristic.
        ptable_L[i] = 21; // dummy val to see which indeces are updated.
    }

    fprintf(stderr, "Start to gen ptable for group L\n");
    ptable_L[cube_to_L_index(&cube)] = 0;

    // calculate the initial index
    int ece = cube_to_ece_index(&cube);
    int coud = cube_to_coud_index(&cube);
    int eofb = cube_to_eofb_index(&cube);
    unsigned int p = 
        (unsigned int)ece
        + (unsigned int)eofb * 495
        + (unsigned int)coud * 495 * 2048;

    for (int depth = 1; depth <= 10; depth++){
        fprintf(stderr, "Searching at depth %i\n", depth);
        DLS_L(ece, eofb, coud, p, depth, 18, 1, ptable_L);
    }
    
    fprintf(stderr, "writing ptable for group L to disk. file: L.dat.\n");
    
    FILE *file = fopen("L.dat", "wb");
    if (!file)
    {
        free(ptable_L);
        return;
    }

    // Save the array
    fwrite(ptable_L, sizeof(uint16_t) * SIZEL, 1, file);

    fclose(file);
    free(ptable_L);
}

/*
static void
DLS(
    cube_t* cube,
    int depth,
    int prev_move,
    int num_moves_done,
    unsigned int (*cube_to_index)(cube_t* cube),
    uint16_t* ptable
){
    if (depth == 0) return;
    unsigned int p;
  
    for (int m = 0; m < NMOVES; m++){
        // do not check redundant sequences, ex.: R R, R L R.
        if (!(m/6 == prev_move/6 && m/3 >= prev_move/3)){
            cube_move_apply_move(cube, m);

            p = cube_to_index(cube);

            // if (ptable[p] == 0 || ptable[p] == num_moves_done){
            if (ptable[p] > num_moves_done || ptable[p] == num_moves_done){
                ptable[p] = (uint16_t)num_moves_done;
                DLS(cube, depth - 1, m, num_moves_done + 1, cube_to_index, ptable);
            }
            cube_move_apply_move(cube, get_inv_move(m));
        }
    }
}
*/

static void
DLS_L(
    int ece,
    int eofb,
    int coud,
    unsigned int p,
    int depth,
    int prev_move,
    int num_moves_done,
    uint16_t* ptable
){
    if (depth == 0) return;
    unsigned int p2;
  
    for (int m = 0; m < NMOVES; m++){
        // do not check redundant sequences, ex.: R R, R L R.
        if (!(m/6 == prev_move/6 && m/3 >= prev_move/3)){
            int ece2 = ece_mtable[ece][m];
            int eofb2 = eofb_mtable[eofb][m];
            int coud2 = coud_mtable[coud][m];

            p2 = 
                (unsigned int)ece2
                + (unsigned int)eofb2 * 495
                + (unsigned int)coud2 * 495 * 2048;

            if (p2 == p) continue;

            if (ptable[p2] >= num_moves_done){
                ptable[p2] = (uint16_t)num_moves_done;
            }
            DLS_L(ece2, eofb2, coud2, p2, depth - 1, m, num_moves_done + 1, ptable);
        }
    }
}


// testing
void gen_ece_mtable(){
  for (int m = 0; m < NMOVES; m++){
    for (int c1 = 0; c1 < 9; c1++){
      for (int c2 = c1 + 1; c2 < 10; c2++){
        for (int c3 = c2 + 1; c3 < 11; c3++){
          for (int c4 = c3 + 1; c4 < 12; c4++){
            cube_t cube = cube_create_new_cube();
            update_edge_perm(&cube.edges[BL], c1);
            update_edge_perm(&cube.edges[BR], c2);
            update_edge_perm(&cube.edges[FR], c3);
            update_edge_perm(&cube.edges[FL], c4);
            
            int i = cube_to_ece_index(&cube);
            cube_move_apply_move(&cube, m);
            int i_after = cube_to_ece_index(&cube);
            
            ece_mtable[i][m] = i_after;
          }
        }
      }
    }
  }
}

void gen_coud_mtable(){
  for (int m = 0; m < NMOVES; m++){
    for (int O1 = 0; O1 < 3; O1++){
      for (int O2 = 0; O2 < 3; O2++){
        for (int O3 = 0; O3 < 3; O3++){
          for (int O4 = 0; O4 < 3; O4++){
            for (int O5 = 0; O5 < 3; O5++){
              for (int O6 = 0; O6 < 3; O6++){
                for (int O7 = 0; O7 < 3; O7++){
                  cube_t cube = cube_create_new_cube();
                  update_corner_orien(&cube.corners[0], O1, 0, 0);
                  update_corner_orien(&cube.corners[1], O2, 0, 0);
                  update_corner_orien(&cube.corners[2], O3, 0, 0);
                  update_corner_orien(&cube.corners[3], O4, 0, 0);
                  update_corner_orien(&cube.corners[4], O5, 0, 0);
                  update_corner_orien(&cube.corners[5], O6, 0, 0);
                  update_corner_orien(&cube.corners[6], O7, 0, 0);
                  update_corner_orien(&cube.corners[7], (3 - (O1 + O2 + O3 + O4 + O5 + O6 + O7) % 3) % 3 , 0, 0);
                  fix_co_lr_ud(&cube);

                  int i = cube_to_coud_index(&cube);
                  cube_move_apply_move(&cube, m);
                  int i_after = cube_to_coud_index(&cube);
                  
                  coud_mtable[i][m] = i_after;
                }
              }
            }
          }
        }
      }
    }
  }
}

void gen_eofb_mtable(){
  for (int m = 0; m < NMOVES; m++){
    for (int O1 = 0; O1 < 2; O1++){
      for (int O2 = 0; O2 < 2; O2++){
        for (int O3 = 0; O3 < 2; O3++){
          for (int O4 = 0; O4 < 2; O4++){
            for (int O5 = 0; O5 < 2; O5++){
              for (int O6 = 0; O6 < 2; O6++){
                for (int O7 = 0; O7 < 2; O7++){
                  for (int O8 = 0; O8 < 2; O8++){
                    for (int O9 = 0; O9 < 2; O9++){
                      for (int O10 = 0; O10 < 2; O10++){
                        for (int O11 = 0; O11 < 2; O11++){
                          cube_t cube = cube_create_new_cube();
                          update_edge_orien(&cube.edges[0], O1, 0, 0);
                          update_edge_orien(&cube.edges[1], O2, 0, 0);
                          update_edge_orien(&cube.edges[2], O3, 0, 0);
                          update_edge_orien(&cube.edges[3], O4, 0, 0);
                          update_edge_orien(&cube.edges[4], O5, 0, 0);
                          update_edge_orien(&cube.edges[5], O6, 0, 0);
                          update_edge_orien(&cube.edges[6], O7, 0, 0);
                          update_edge_orien(&cube.edges[7], O8, 0, 0);
                          update_edge_orien(&cube.edges[8], O9, 0, 0);
                          update_edge_orien(&cube.edges[9], O10, 0, 0);
                          update_edge_orien(&cube.edges[10], O11, 0, 0);
                          update_edge_orien(&cube.edges[11], (2 - (O1 + O2 + O3 + O4 + O5 + O6 + O7 + O8 + O9 + O10 + O11) % 2) % 2, 0, 0);
                          fix_eo_lr_ud(&cube);

                          int i = cube_to_eofb_index(&cube);
                          cube_move_apply_move(&cube, m);
                          int i_after = cube_to_eofb_index(&cube);

                          eofb_mtable[i][m] = i_after;
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

