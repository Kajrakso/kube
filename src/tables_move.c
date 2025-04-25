#include "_tables.h"

uint16_t move_table_corner_transformation[NMOVES][NCORNERCUBIES];
uint16_t move_table_edge_transformation[NMOVES][NEDGECUBIES];

uint64_t ece_mtable[NECE][NMOVES];
uint64_t eofb_mtable[NEO][NMOVES];
uint64_t coud_mtable[NCO][NMOVES];
uint64_t ccu_mtable[NCCU][NMOVES];

enum facemove {
  U, D,
  L, R,
  F, B
};

static const int edge_perm_cycles[NFACES][4] = {
    [U] = {UB, UR, UF, UL},
    [D] = {DF, DR, DB, DL},
    [L] = {UL, FL, DL, BL},
    [R] = {UR, BR, DR, FR},
    [F] = {UF, FR, DF, FL},
    [B] = {UB, BL, DB, BR},
};

static const int edge_orient_change[NAXES][NFACES] = {
    [FB] = {
        /* U, D, L, R, F, B*/
        0, 0, 0, 0, 1, 1
    },
    [LR] = {
        /* U, D, L, R, F, B*/
        0, 0, 1, 1, 0, 0
    },
    [UD] = {
        /* U, D, L, R, F, B*/
        1, 1, 0, 0, 0, 0
    },
};

static const int corner_perm_cycles[NFACES][4] = {
    [U] = {UBL, UBR, UFR, UFL},
    [D] = {DFL, DFR, DBR, DBL},
    [L] = {UBL, UFL, DFL, DBL},
    [R] = {UFR, UBR, DBR, DFR},
    [F] = {UFL, UFR, DFR, DFL},
    [B] = {UBR, UBL, DBL, DBR},
};

static const int corner_orient_change[NAXES][NFACES][4] = {
    [FB] = {
        [U] = {1, 2, 1, 2},
        [D] = {1, 2, 1, 2},
        [L] = {2, 1, 2, 1},
        [R] = {2, 1, 2, 1},
        [F] = {0, 0, 0, 0},
        [B] = {0, 0, 0, 0},
    },
    [LR] = {
        [U] = {2, 1, 2, 1},
        [D] = {2, 1, 2, 1},
        [L] = {0, 0, 0, 0},
        [R] = {0, 0, 0, 0},
        [F] = {2, 1, 2, 1},
        [B] = {2, 1, 2, 1},
    },
    [UD] = {
        [U] = {0, 0, 0, 0},
        [D] = {0, 0, 0, 0},
        [L] = {1, 2, 1, 2},
        [R] = {1, 2, 1, 2},
        [F] = {1, 2, 1, 2},
        [B] = {1, 2, 1, 2},
    },
};


/* Fills the tables with default values.
This have to be done before we generate the
actual moves. */
void initialize_move_tables(){
    for (int m = 0; m < NMOVES; m++){
        for (uint16_t c = 0; c < NCORNERCUBIES; c++){
            move_table_corner_transformation[m][c] = c;
        }
        for (uint16_t e = 0; e < NEDGECUBIES; e++){
            move_table_edge_transformation[m][e] = e;
        }
    }
}

/* NB: we gen moves for states that are impossible.
 as a first try this is probably ok, but should be thought about. */
static inline void gen_moves_corners(int face, int turns, int piece){
    int move = NTWISTS * face + turns - 1;
    bool is_quarter_move = (turns == 1 || turns == 3);

    for (int co_fb = 0; co_fb < 3; co_fb++){
        for (int co_lr = 0; co_lr < 3; co_lr++){
            for (int co_ud = 0; co_ud < 3; co_ud++){
                int cp = corner_perm_cycles[face][piece];
                
                int new_cp = corner_perm_cycles[face][(piece + turns) % 4];
                int new_co_fb = (is_quarter_move) ? (co_fb + corner_orient_change[FB][face][piece]) % 3 : co_fb;
                int new_co_lr = (is_quarter_move) ? (co_lr + corner_orient_change[LR][face][piece]) % 3 : co_lr;
                int new_co_ud = (is_quarter_move) ? (co_ud + corner_orient_change[UD][face][piece]) % 3 : co_ud;

                uint16_t old_c = build_corner(cp, co_fb, co_lr, co_ud);
                uint16_t new_c = build_corner(new_cp, new_co_fb, new_co_lr, new_co_ud);

                move_table_corner_transformation[move][old_c] = new_c;
            }
        }
    }
}

/* NB: we gen moves for states that are impossible.
 as a first try this is probably ok, but should be thought about. */
static inline void gen_moves_edges(int face, int turns, int piece){
    int move = NTWISTS * face + turns - 1;
    bool is_quarter_move = (turns == 1 || turns == 3);

    for (int eo_fb = 0; eo_fb < 2; eo_fb++){
        for (int eo_lr = 0; eo_lr < 2; eo_lr++){
            for (int eo_ud = 0; eo_ud < 2; eo_ud++){
                int ep = edge_perm_cycles[face][piece];

                int new_ep = edge_perm_cycles[face][(piece + turns) % 4];
                int new_eo_fb = (is_quarter_move) ? (edge_orient_change[FB][face] ^ eo_fb) : eo_fb;
                int new_eo_lr = (is_quarter_move) ? (edge_orient_change[LR][face] ^ eo_lr) : eo_lr;
                int new_eo_ud = (is_quarter_move) ? (edge_orient_change[UD][face] ^ eo_ud) : eo_ud;

                uint16_t old_e = build_edge(ep, eo_fb, eo_lr, eo_ud);
                uint16_t new_e = build_edge(new_ep, new_eo_fb, new_eo_lr, new_eo_ud);

                move_table_edge_transformation[move][old_e] = new_e;
            }
        }
    }
}

/* Basically just does moves so that we can look them up later.
 You have to run `initialize_move_tables` first,
 because this function only updates the values
 of pieces that *actually* move under a twist. */
void gen_move_tables(){
    // faces: F, B, R, L, U, D.
    for (int f = 0; f < NFACES; f++){
        // number of twists: i.e. R1, R2, R3
        for (int t = 1; t <= NTWISTS; t++){
            // four pieces on a face. i.e: UBL, UBR, UFR, UFL
            for (int p = 0; p < 4; p++){
                gen_moves_corners(f, t, p);
                gen_moves_edges(f, t, p);
            }
        }
    }
}

void
gen_ece_mtable(){
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
            
            uint64_t i = cube_to_ece_index(&cube);
            cube_move_apply_move(&cube, m);
            uint64_t ii = cube_to_ece_index(&cube);
            
            ece_mtable[i][m] = ii;
          }
        }
      }
    }
  }
}

void
gen_coud_mtable(){
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

                  uint64_t i = cube_to_coud_index(&cube);
                  cube_move_apply_move(&cube, m);
                  uint64_t ii = cube_to_coud_index(&cube);
                  
                  coud_mtable[i][m] = ii;
                }
              }
            }
          }
        }
      }
    }
  }
}

void
gen_eofb_mtable(){
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

                          uint64_t i = cube_to_eofb_index(&cube);
                          cube_move_apply_move(&cube, m);
                          uint64_t ii = cube_to_eofb_index(&cube);

                          eofb_mtable[i][m] = ii;
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

void
gen_ccu_mtable(){
  for (int c1 = 0; c1 < 5; c1++){
    for (int c2 = c1 + 1; c2 < 6; c2++){
      for (int c3 = c2 + 1; c3 < 7; c3++){
        for (int c4 = c3 + 1; c4 < 8; c4++){
          for (int m = 0; m < NMOVES; m++){
            cube_t cube = cube_create_new_cube();
            int e;

            e = which_corner_at_pos(c1, &cube);
            update_corner_perm(&cube.corners[UBL], c1);
            update_corner_perm(&cube.corners[e], UBL);

            e = which_corner_at_pos(c2, &cube);
            update_corner_perm(&cube.corners[UBR], c2);
            update_corner_perm(&cube.corners[e], UBR);

            e = which_corner_at_pos(c3, &cube);
            update_corner_perm(&cube.corners[UFR], c3);
            update_corner_perm(&cube.corners[e], UFR);

            e = which_corner_at_pos(c4, &cube);
            update_corner_perm(&cube.corners[UFL], c4);
            update_corner_perm(&cube.corners[e], UFL);

            fix_eo_lr_ud(&cube);
            fix_co_lr_ud(&cube);
            
            uint64_t i = cube_to_ccu_index(&cube);
            cube_move_apply_move(&cube, m);
            uint64_t ii = cube_to_ccu_index(&cube);
            
            ccu_mtable[i][m] = ii;
          }
        }
      }
    }
  }
}
