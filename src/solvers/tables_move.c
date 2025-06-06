#include "_index.h"

uint32_t move_mask[NMOVES + 1];
uint64_t move_table_ccu_index[NCCU][NMOVES];
uint64_t move_table_coud_index[NCO][NMOVES];
uint64_t move_table_ece_index[NECE][NMOVES];
uint64_t move_table_eofb_index[NEO][NMOVES];

void gen_move_mask() {
  int m, pm;
  for (pm = 0; pm < NMOVES; pm++) {
    move_mask[pm] = 0;
    for (m = 0; m < NMOVES; m++) {
      if (!(m / 6 == pm / 6 && m / 3 >= pm / 3)) {
        move_mask[pm] |= 1 << m;
      }
    }
    for (m = 0; m < NMOVES; m++) {
      move_mask[18] |= 1 << m;
    }
  }
}

void movemask_remove_move(int move) {
  for (int pm = 0; pm < NMOVES + 1; pm++) {
    move_mask[pm] &= ~((uint32_t)7 << 3 * (move / 3));
  }
}

void gen_move_table_ece_index() {
  for (int c1 = 0; c1 < 9; c1++) {
    for (int c2 = c1 + 1; c2 < 10; c2++) {
      for (int c3 = c2 + 1; c3 < 11; c3++) {
        for (int c4 = c3 + 1; c4 < 12; c4++) {
          for (int m = 0; m < NMOVES; m++) {
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

            move_table_ece_index[i][m] = ii;
          }
        }
      }
    }
  }
}

void gen_move_table_coud_index() {
  for (int O1 = 0; O1 < 3; O1++) {
    for (int O2 = 0; O2 < 3; O2++) {
      for (int O3 = 0; O3 < 3; O3++) {
        for (int O4 = 0; O4 < 3; O4++) {
          for (int O5 = 0; O5 < 3; O5++) {
            for (int O6 = 0; O6 < 3; O6++) {
              for (int O7 = 0; O7 < 3; O7++) {
                for (int m = 0; m < NMOVES; m++) {
                  cube_t cube = cube_create_new_cube();
                  update_corner_orien(&cube.corners[0], O1, 0, 0);
                  update_corner_orien(&cube.corners[1], O2, 0, 0);
                  update_corner_orien(&cube.corners[2], O3, 0, 0);
                  update_corner_orien(&cube.corners[3], O4, 0, 0);
                  update_corner_orien(&cube.corners[4], O5, 0, 0);
                  update_corner_orien(&cube.corners[5], O6, 0, 0);
                  update_corner_orien(&cube.corners[6], O7, 0, 0);
                  update_corner_orien(
                      &cube.corners[7],
                      (3 - (O1 + O2 + O3 + O4 + O5 + O6 + O7) % 3) % 3, 0, 0);
                  fix_eo_lr_ud(&cube);
                  fix_co_lr_ud(&cube);

                  uint64_t i = cube_to_coud_index(&cube);
                  cube_move_apply_move(&cube, m);
                  uint64_t ii = cube_to_coud_index(&cube);

                  move_table_coud_index[i][m] = ii;
                }
              }
            }
          }
        }
      }
    }
  }
}

void gen_move_table_eofb_index() {
  for (int O1 = 0; O1 < 2; O1++) {
    for (int O2 = 0; O2 < 2; O2++) {
      for (int O3 = 0; O3 < 2; O3++) {
        for (int O4 = 0; O4 < 2; O4++) {
          for (int O5 = 0; O5 < 2; O5++) {
            for (int O6 = 0; O6 < 2; O6++) {
              for (int O7 = 0; O7 < 2; O7++) {
                for (int O8 = 0; O8 < 2; O8++) {
                  for (int O9 = 0; O9 < 2; O9++) {
                    for (int O10 = 0; O10 < 2; O10++) {
                      for (int O11 = 0; O11 < 2; O11++) {
                        for (int m = 0; m < NMOVES; m++) {
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
                          update_edge_orien(&cube.edges[11],
                                            (O1 + O2 + O3 + O4 + O5 + O6 + O7 +
                                             O8 + O9 + O10 + O11) %
                                                2,
                                            0, 0);
                          fix_eo_lr_ud(&cube);
                          fix_co_lr_ud(&cube);

                          uint64_t i = cube_to_eofb_index(&cube);
                          cube_move_apply_move(&cube, m);
                          uint64_t ii = cube_to_eofb_index(&cube);

                          move_table_eofb_index[i][m] = ii;
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

void gen_move_table_ccu_index() {
  for (int c1 = 0; c1 < 5; c1++) {
    for (int c2 = c1 + 1; c2 < 6; c2++) {
      for (int c3 = c2 + 1; c3 < 7; c3++) {
        for (int c4 = c3 + 1; c4 < 8; c4++) {
          for (int m = 0; m < NMOVES; m++) {
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

            move_table_ccu_index[i][m] = ii;
          }
        }
      }
    }
  }
}
