#include "moves.h"

// todo: make there arrays constants. 

int edge_perm_cycles[NFACES][4] = {
    [U] = {UB, UR, UF, UL},
    [D] = {DF, DR, DB, DL},
    [L] = {UL, FL, DL, BL},
    [R] = {UR, BR, DR, FR},
    [F] = {UF, FR, DF, FL},
    [B] = {UB, BL, DB, BR},
};

int edge_orient_change[NAXES][NFACES] = {
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

int corner_perm_cycles[NFACES][4] = {
    [U] = {UBL, UBR, UFR, UFL},
    [D] = {DFL, DFR, DBR, DBL},
    [L] = {UBL, UFL, DFL, DBL},
    [R] = {UFR, UBR, DBR, DFR},
    [F] = {UFL, UFR, DFR, DFL},
    [B] = {UBR, UBL, DBL, DBR},
};

int corner_orient_change[NAXES][NFACES][4] = {
    [FB] = {
        [U] = {0, 0, 0, 0},
        [D] = {0, 0, 0, 0},
        [L] = {1, 2, 1, 2},
        [R] = {1, 2, 1, 2},
        [F] = {1, 2, 1, 2},
        [B] = {1, 2, 1, 2},
    },
    [LR] = {
        [U] = {0, 0, 0, 0},
        [D] = {0, 0, 0, 0},
        [L] = {1, 2, 1, 2},
        [R] = {1, 2, 1, 2},
        [F] = {1, 2, 1, 2},
        [B] = {1, 2, 1, 2},
    },
    [UD] = {
        [U] = {1, 2, 1, 2},
        [D] = {1, 2, 1, 2},
        [L] = {2, 1, 2, 1},
        [R] = {2, 1, 2, 1},
        [F] = {0, 0, 0, 0},
        [B] = {0, 0, 0, 0},
    },
};

/* fills the tables with default values */
void initialize_move_tables(uint16_t corner_trans[NMOVES][NCORNERCUBIES], uint16_t edge_trans[NMOVES][NEDGECUBIES]){
    /* initialize the arrays: */
    for (int m = 0; m < NMOVES; m++){
        for (int c = 0; c < NCORNERCUBIES; c++){
            corner_trans[m][c] = c;
        }
        for (int c = 0; c < NEDGECUBIES; c++){
            edge_trans[m][c] = c;
        }
    }
}

/* Basically just does moves so that we can look them up later.

 uint16_t corner_trans[NMOVES][NCORNERCUBIES]
 uint16_t edge_trans[NMOVES][NEDGECUBIES]
 */
// todo: should split this into two functions.
void gen_move_tables(uint16_t corner_trans[NMOVES][NCORNERCUBIES], uint16_t edge_trans[NMOVES][NEDGECUBIES]){
    /* Now update the elements that
    change when we do a move */
    // faces
    for (int f = 0; f < NFACES; f++){
        // moves: i.e. R, R2, R3
        for (int m = 0; m < NTWISTS; m++){
            int move = NTWISTS*f + m;
            bool is_quarter_move = (m == 0 || m == 2);

            // each piece on the face f
            for (int p = 0; p < 4; p++){
                // corners 
                // NB: we gen moves for states that are impossible.
                // as a first try this is probably ok, but should be thought about.
                for (int co_fb = 0; co_fb < 3; co_fb++){
                    for (int co_lr = 0; co_lr < 3; co_lr++){
                        for (int co_ud = 0; co_ud < 3; co_ud++){
                            int cp = corner_perm_cycles[f][p];
                            int new_cp = corner_perm_cycles[f][(p + m + 1) % 4];
                            int new_co_fb = (is_quarter_move) ? (co_fb + corner_orient_change[FB][f][p]) % 3 : co_fb;
                            int new_co_lr = (is_quarter_move) ? (co_lr + corner_orient_change[LR][f][p]) % 3 : co_lr;
                            int new_co_ud = (is_quarter_move) ? (co_ud + corner_orient_change[UD][f][p]) % 3 : co_ud;

                            uint16_t old_c = build_corner(cp, co_fb, co_lr, co_ud);
                            uint16_t new_c = build_corner(new_cp, new_co_fb, new_co_lr, new_co_ud);

                            corner_trans[move][old_c] = new_c;
                        }
                    }
                }

                // edges
                for (int eo_fb = 0; eo_fb < 2; eo_fb++){
                    for (int eo_lr = 0; eo_lr < 2; eo_lr++){
                        for (int eo_ud = 0; eo_ud < 2; eo_ud++){
                            int ep = edge_perm_cycles[f][p];
                            int new_ep = edge_perm_cycles[f][(p + m + 1) % 4];
                            int new_eo_fb = (is_quarter_move) ? (edge_orient_change[FB][f] ^ eo_fb) : eo_fb;
                            int new_eo_lr = (is_quarter_move) ? (edge_orient_change[LR][f] ^ eo_lr) : eo_lr;
                            int new_eo_ud = (is_quarter_move) ? (edge_orient_change[UD][f] ^ eo_ud) : eo_ud;

                            uint16_t old_e = build_edge(ep, eo_fb, eo_lr, eo_ud);
                            uint16_t new_e = build_edge(new_ep, new_eo_fb, new_eo_lr, new_eo_ud);

                            edge_trans[move][old_e] = new_e;
                        }
                    }
                }
            }
        }
    }
}


// todo: look into making the mtables global variables.
// todo: use *move* instead of a *facemove* enum.
// todo: 
void make_move(cube_t* cube, facemove move, int turns, uint16_t corner_trans[NMOVES][NCORNERCUBIES], uint16_t edge_trans[NMOVES][NEDGECUBIES]){
    uint16_t* p = corner_trans[3*move + turns];
    
    cube->corners[0] = p[cube->corners[0]];
    cube->corners[1] = p[cube->corners[1]];
    cube->corners[2] = p[cube->corners[2]];
    cube->corners[3] = p[cube->corners[3]];
    cube->corners[4] = p[cube->corners[4]];
    cube->corners[5] = p[cube->corners[5]];
    cube->corners[6] = p[cube->corners[6]];
    cube->corners[7] = p[cube->corners[7]];
    
    p = edge_trans[3*move + turns];

    cube->edges[0] = p[cube->edges[0]];
    cube->edges[1] = p[cube->edges[1]];
    cube->edges[2] = p[cube->edges[2]];
    cube->edges[3] = p[cube->edges[3]];
    cube->edges[4] = p[cube->edges[4]];
    cube->edges[5] = p[cube->edges[5]];
    cube->edges[6] = p[cube->edges[6]];
    cube->edges[7] = p[cube->edges[7]];
    cube->edges[8] = p[cube->edges[8]];
    cube->edges[9] = p[cube->edges[9]];
    cube->edges[10] = p[cube->edges[10]];
    cube->edges[11] = p[cube->edges[11]];
}