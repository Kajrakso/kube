#include "_tables.h"
#include "_index.h"
#include "stdio.h"

uint32_t ece_mtable[NECE][NMOVES];
uint32_t coud_mtable[NCO][NMOVES];
uint32_t eofb_mtable[NEO][NMOVES];

void ptable_set_val(uint32_t i, uint8_t p, uint8_t* ptable){
  if (i % 2 == 0){
    ptable[i >> 1] = (ptable[i >> 1] & 0xF0) | (p & 0x0F);
  } else {
    ptable[i >> 1] = (ptable[i >> 1] & 0x0F) | ((p & 0x0F) << 4);
  }
}

uint8_t ptable_read_val(uint32_t i, uint8_t* ptable){
  if (i % 2 == 0) {
    return ptable[i >> 1] & 0x0F;
  } else {
    return (ptable[i >> 1] >> 4) & 0x0F;
  }
}

void
gen_ptable_L()
{
  cube_t cube = cube_create_new_cube();
  cube_tables_generate();
  precompute_combinatorials();

  fprintf(stderr, "Generating move tables\n");
  fprintf(stderr, "gen_ece_mtable\n");
  gen_ece_mtable();
  fprintf(stderr, "gen_coud_mtable\n");
  gen_coud_mtable();
  fprintf(stderr, "gen_eofb_mtable\n");
  gen_eofb_mtable();

  uint8_t* ptable_L = malloc(sizeof(uint8_t) * SIZE_PTABLE_L);

  fprintf(stderr, "Initializing ptable for group L\n");

  // calculate the initial index
  uint32_t ece = cube_to_ece_index(&cube);
  uint32_t coud = cube_to_coud_index(&cube);
  uint32_t eofb = cube_to_eofb_index(&cube);
  
  for (uint32_t i = 0; i < SIZEL; i++){
    // ptable_L[i] = 15; // dummy val to see which indeces are updated.
    ptable_set_val(i, 15, ptable_L);
  }
  
  uint32_t p = cube_to_L_index(&cube);
  ptable_set_val(p, 0, ptable_L);
  // ptable_L[p] = 0;  // and set it to 0

/* TODO: improve table gen! https://github.com/sebastianotronto/h48/blob/master/doc/h48.md#4-bits-tables-for-h0-and-h11 */
  
  for (int depth = 0; depth < 9; depth++){
    fprintf(stderr, "Searching at depth %i\n", depth);
    DLS_L(ece, eofb, coud, p, depth, 18, 0, ptable_L);
  }

  for (uint8_t k = 9; k < 12; k++){
    fprintf(stderr, "Searching at depth %i\n", k);
    for (coud = 0; coud < NCO; coud++){
      for (eofb = 0; eofb < NEO; eofb++){
        for (ece = 0; ece < NECE; ece++){
          uint32_t p = ece_eofb_coud_to_L_index(ece, eofb, coud);
          if (ptable_read_val(p, ptable_L) == 15){
            for (int m = 0; m < NMOVES; m++){
              uint32_t ece2 = ece_mtable[ece][m];
              uint32_t eofb2 = eofb_mtable[eofb][m];
              uint32_t coud2 = coud_mtable[coud][m];
              uint32_t p2 = ece_eofb_coud_to_L_index(ece2, eofb2, coud2);
            
              if (ptable_read_val(p2, ptable_L) == k - 1){
                ptable_set_val(p, k, ptable_L);
              }
            }
          }
        }
      }
    }
  }

  // set the remaining cosets to 12,
  for (coud = 0; coud < NCO; coud++){
    for (eofb = 0; eofb < NEO; eofb++){
      for (ece = 0; ece < NECE; ece++){
        uint32_t p = ece_eofb_coud_to_L_index(ece, eofb, coud);
        if (ptable_read_val(p, ptable_L) == 15){ ptable_set_val(p, 12, ptable_L); }
      }
    }
  }

/* end of TODO */

  fprintf(stderr, "writing ptable for group L to disk. file: L.dat.\n");
  
  FILE *file = fopen("L.dat", "wb");
  if (!file) {
    free(ptable_L);
    return;
  }

  // Save the array
  fwrite(ptable_L, sizeof(uint8_t) * SIZE_PTABLE_L, 1, file);

  fclose(file);
  free(ptable_L);
}

void
DLS_L(
  uint32_t ece,
  uint32_t eofb,
  uint32_t coud,
  uint32_t p,
  int depth,
  int prev_move,
  int num_moves_done,
  uint8_t* ptable
){
    if (depth == 0) return;
    uint32_t p2;
  
    for (int m = 0; m < NMOVES; m++){
        // do not check redundant sequences, ex.: R R, R L R.
        if (!(m/6 == prev_move/6 && m/3 >= prev_move/3)){
          uint32_t ece2 = ece_mtable[ece][m];
          uint32_t eofb2 = eofb_mtable[eofb][m];
          uint32_t coud2 = coud_mtable[coud][m];
          p2 = ece_eofb_coud_to_L_index(ece2, eofb2, coud2);

          if (p2 == p) continue;

          if (ptable_read_val(p2, ptable) > num_moves_done + 1) {
            ptable_set_val(p2, (uint8_t)(num_moves_done + 1), ptable);
          }
          DLS_L(ece2, eofb2, coud2, p2, depth - 1, m, num_moves_done + 1, ptable);
        }
    }
}

void gen_ece_mtable(){
  for (int c1 = 0; c1 < 9; c1++){
    for (int c2 = c1 + 1; c2 < 10; c2++){
      for (int c3 = c2 + 1; c3 < 11; c3++){
        for (int c4 = c3 + 1; c4 < 12; c4++){
          for (int m = 0; m < NMOVES; m++){
            cube_t cube = cube_create_new_cube();
            int e;

            e = which_edge_at_pos(c1, &cube);
            update_edge_perm(&cube.edges[BL], c1);
            update_edge_perm(&cube.edges[e], BL);

            e = which_edge_at_pos(c2, &cube);
            update_edge_perm(&cube.edges[BR], c2);
            update_edge_perm(&cube.edges[e], BR);

            e = which_edge_at_pos(c3, &cube);
            update_edge_perm(&cube.edges[FR], c3);
            update_edge_perm(&cube.edges[e], FR);

            e = which_edge_at_pos(c4, &cube);
            update_edge_perm(&cube.edges[FL], c4);
            update_edge_perm(&cube.edges[e], FL);
            fix_eo_lr_ud(&cube);
            fix_co_lr_ud(&cube);
            
            uint32_t i = cube_to_ece_index(&cube);
            cube_move_apply_move(&cube, m);
            uint32_t i_after = cube_to_ece_index(&cube);
            
            ece_mtable[i][m] = i_after;
          }
        }
      }
    }
  }
}

void gen_coud_mtable(){
  for (int O1 = 0; O1 < 3; O1++){
    for (int O2 = 0; O2 < 3; O2++){
      for (int O3 = 0; O3 < 3; O3++){
        for (int O4 = 0; O4 < 3; O4++){
          for (int O5 = 0; O5 < 3; O5++){
            for (int O6 = 0; O6 < 3; O6++){
              for (int O7 = 0; O7 < 3; O7++){
                for (int m = 0; m < NMOVES; m++){
                  cube_t cube = cube_create_new_cube();
                  update_corner_orien(&cube.corners[0], O1, 0, 0);
                  update_corner_orien(&cube.corners[1], O2, 0, 0);
                  update_corner_orien(&cube.corners[2], O3, 0, 0);
                  update_corner_orien(&cube.corners[3], O4, 0, 0);
                  update_corner_orien(&cube.corners[4], O5, 0, 0);
                  update_corner_orien(&cube.corners[5], O6, 0, 0);
                  update_corner_orien(&cube.corners[6], O7, 0, 0);
                  update_corner_orien(&cube.corners[7], (3 - (O1 + O2 + O3 + O4 + O5 + O6 + O7) % 3) % 3 , 0, 0);
                  fix_eo_lr_ud(&cube);
                  fix_co_lr_ud(&cube);

                  uint32_t i = cube_to_coud_index(&cube);
                  cube_move_apply_move(&cube, m);
                  uint32_t i_after = cube_to_coud_index(&cube);
                  
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
                        for (int m = 0; m < NMOVES; m++){
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
                          update_edge_orien(&cube.edges[11], (O1 + O2 + O3 + O4 + O5 + O6 + O7 + O8 + O9 + O10 + O11) % 2, 0, 0);
                          fix_eo_lr_ud(&cube);
                          fix_co_lr_ud(&cube);

                          uint32_t i = cube_to_eofb_index(&cube);
                          cube_move_apply_move(&cube, m);
                          uint32_t i_after = cube_to_eofb_index(&cube);

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

