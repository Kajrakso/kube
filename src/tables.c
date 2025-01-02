#include "_tables.h"

/* moves */
uint16_t move_table_corner_transformation[NMOVES][NCORNERCUBIES];
uint16_t move_table_edge_transformation[NMOVES][NEDGECUBIES];

/* symmetries */
uint16_t sym_table_corner_transformation[NSYMS][NCORNERCUBIES];
uint16_t sym_table_edge_transformation[NSYMS][NEDGECUBIES];

/* ---------------------------- public ---------------------------- */

void cube_tables_generate(){
    // gen moves
    initialize_move_tables();
    gen_move_tables();

    // gen sym
    initialize_sym_tables();
    gen_sym_tables();
}

bool cube_tables_save(const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) return false;

    // Save the move_table_corner_transformation array
    if (fwrite(move_table_corner_transformation, sizeof(move_table_corner_transformation), 1, file) != 1) {
        fclose(file);
        return false;
    }

    // Save the move_table_edge_transformation array
    if (fwrite(move_table_edge_transformation, sizeof(move_table_edge_transformation), 1, file) != 1) {
        fclose(file);
        return false;
    }

    // Save the sym_table_corner_transformation array
    if (fwrite(sym_table_corner_transformation, sizeof(sym_table_corner_transformation), 1, file) != 1) {
        fclose(file);
        return false;
    }

    // Save the sym_table_edge_transformation array
    if (fwrite(sym_table_edge_transformation, sizeof(sym_table_edge_transformation), 1, file) != 1) {
        fclose(file);
        return false;
    }

    fclose(file);
    return true;
}

bool cube_tables_load(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) return false;

    // Load the move_table_corner_transformation array
    if (fread(move_table_corner_transformation, sizeof(move_table_corner_transformation), 1, file) != 1) {
        fclose(file);
        return false;
    }

    // Load the move_table_edge_transformation array
    if (fread(move_table_edge_transformation, sizeof(move_table_edge_transformation), 1, file) != 1) {
        fclose(file);
        return false;
    }

    // Load the sym_table_corner_transformation array
    if (fread(sym_table_corner_transformation, sizeof(sym_table_corner_transformation), 1, file) != 1) {
        fclose(file);
        return false;
    }

    // Load the sym_table_edge_transformation array
    if (fread(sym_table_edge_transformation, sizeof(sym_table_edge_transformation), 1, file) != 1) {
        fclose(file);
        return false;
    }

    fclose(file);
    return true;
}

/* ------------------------------ private --------------------------- */

/* Fills the tables with default values.
This have to be done before we generate the
actual moves. */
static void initialize_move_tables(){
    for (int m = 0; m < NMOVES; m++){
        for (int c = 0; c < NCORNERCUBIES; c++){
            move_table_corner_transformation[m][c] = c;
        }
        for (int c = 0; c < NEDGECUBIES; c++){
            move_table_edge_transformation[m][c] = c;
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
static void gen_move_tables(){
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

/* Fills the tables with default values.
This have to be done before we generate the
actual conjugations. */
static void initialize_sym_tables(){
    for (int s = 0; s < NSYMS; s++){
        for (int c = 0; c < NCORNERCUBIES; c++){
            sym_table_corner_transformation[s][c] = c;
        }
        for (int c = 0; c < NEDGECUBIES; c++){
            sym_table_edge_transformation[s][c] = c;
        }
    }
}

/*
    todo: explain how we go from integer to element of O_h
    20 for-loops. woho!
*/
static void gen_sym_tables(){
    /* The idea is that we place
    each possible cubie somewhere
    on the cube and then act on 
    the cube by the symmetry given
    by (x1, x2, x3, x4). */
    cube_t cube = cube_create_new_cube();

    for (int x4 = 0; x4 < 2; x4++){
        for (int x3 = 0; x3 < 2; x3++){
            for (int x2 = 0; x2 < 3; x2++){
                for (int x1 = 0; x1 < 4; x1++){
                    for (int cp = 0; cp < NCORNERS; cp++){
                        for (int co_fb = 0; co_fb < 3; co_fb++){
                            for (int co_lr = 0; co_lr < 3; co_lr++){
                                for (int co_ud = 0; co_ud < 3; co_ud++){    
                                    uint16_t old_c = build_corner(cp, co_fb, co_lr, co_ud);
                                    
                                    int c_we_follow = which_corner_at_pos(cp, &cube);
                                    cube.corners[c_we_follow] = old_c;

                                    for (int l = 0; l < x4; l++){
                                        do_inversion(&cube);
                                    }
                                    for (int i = 0; i < x1; i++){
                                        do_y_rot(&cube);
                                    }
                                    for (int j = 0; j < x2; j++){
                                        do_y_rot(&cube);
                                        do_z_rot(&cube);
                                    }
                                    for (int k = 0; k < x3; k++){
                                        do_y_rot(&cube);do_y_rot(&cube);
                                        do_z_rot(&cube);do_z_rot(&cube);do_z_rot(&cube);
                                    }

                                    uint16_t new_c = cube.corners[c_we_follow];

                                    sym_table_corner_transformation[24*x4 + 12*x3 + 4*x2 + x1][old_c] = new_c;
                                }
                            }
                        }
                    }
                    for (int ep = 0; ep < NEDGES; ep++){
                        for (int eo_fb = 0; eo_fb < 2; eo_fb++){
                            for (int eo_lr = 0; eo_lr < 2; eo_lr++){
                                for (int eo_ud = 0; eo_ud < 2; eo_ud++){
                                    uint16_t old_e = build_edge(ep, eo_fb, eo_lr, eo_ud);

                                    int e_we_follow = which_edge_at_pos(ep, &cube);
                                    cube.edges[e_we_follow] = old_e;
                                    
                                    for (int l = 0; l < x4; l++){
                                        do_inversion(&cube);
                                    }
                                    for (int i = 0; i < x1; i++){
                                        do_y_rot(&cube);
                                    }
                                    for (int j = 0; j < x2; j++){
                                        do_y_rot(&cube);
                                        do_z_rot(&cube);
                                    }
                                    for (int k = 0; k < x3; k++){
                                        do_y_rot(&cube);do_y_rot(&cube);
                                        do_z_rot(&cube);do_z_rot(&cube);do_z_rot(&cube);
                                    }

                                    uint16_t new_e = cube.edges[e_we_follow];

                                    sym_table_edge_transformation[24*x4 + 12*x3 + 4*x2 + x1][old_e] = new_e;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

static void do_y_rot(cube_t* cube){
    cube_t cube_y = cube_create_new_cube();

    cube_move_apply_move(&cube_y, U1);
    cube_move_apply_move(&cube_y, D3);

    update_edge_perm(&(cube_y.edges[FL]), BL);
    update_edge_orien(&(cube_y.edges[FL]), 1, 1, 1);
    
    update_edge_perm(&(cube_y.edges[BL]), BR);
    update_edge_orien(&(cube_y.edges[BL]), 1, 1, 1);
    
    update_edge_perm(&(cube_y.edges[BR]), FR);
    update_edge_orien(&(cube_y.edges[BR]), 1, 1, 1);
    
    update_edge_perm(&(cube_y.edges[FR]), FL);
    update_edge_orien(&(cube_y.edges[FR]), 1, 1, 1);

    *cube = cube_operation_compose(*cube, cube_y);
}

static void do_z_rot(cube_t* cube){
    cube_t cube_z = cube_create_new_cube();

    cube_move_apply_move(&cube_z, F1);
    cube_move_apply_move(&cube_z, B3);

    update_edge_perm(&(cube_z.edges[UL]), UR);
    update_edge_orien(&(cube_z.edges[UL]), 1, 1, 1);
    
    update_edge_perm(&(cube_z.edges[UR]), DR);
    update_edge_orien(&(cube_z.edges[UR]), 1, 1, 1);
    
    update_edge_perm(&(cube_z.edges[DR]), DL);
    update_edge_orien(&(cube_z.edges[DR]), 1, 1, 1);
    
    update_edge_perm(&(cube_z.edges[DL]), UL);
    update_edge_orien(&(cube_z.edges[DL]), 1, 1, 1);

    *cube = cube_operation_compose(*cube, cube_z);
}

static int which_edge_at_pos(int pos, cube_t* cube){
    for (int k = 0; k < NEDGES; k++){
      if (extract_edge_perm(cube->edges[k]) == pos) return k;
    }
    return -1;
}

static int which_corner_at_pos(int pos, cube_t* cube){
    for (int k = 0; k < NCORNERS; k++){
      if (extract_corner_perm(cube->corners[k]) == pos) return k;
    }
    return -1;
}

static void do_inversion(cube_t* cube){
    int temp;
    int e1, e2;

    e1 = which_edge_at_pos(UF, cube);
    e2 = which_edge_at_pos(DB, cube);
    update_edge_perm(&(cube->edges[e1]), DB);
    update_edge_perm(&(cube->edges[e2]), UF);

    e1 = which_edge_at_pos(UL, cube);
    e2 = which_edge_at_pos(DR, cube);
    update_edge_perm(&(cube->edges[e1]), DR);
    update_edge_perm(&(cube->edges[e2]), UL);

    e1 = which_edge_at_pos(UR, cube);
    e2 = which_edge_at_pos(DL, cube);
    update_edge_perm(&(cube->edges[e1]), DL);
    update_edge_perm(&(cube->edges[e2]), UR);
    
    e1 = which_edge_at_pos(UB, cube);
    e2 = which_edge_at_pos(DF, cube);
    update_edge_perm(&(cube->edges[e1]), DF);
    update_edge_perm(&(cube->edges[e2]), UB);
    
    e1 = which_edge_at_pos(FL, cube);
    e2 = which_edge_at_pos(BR, cube);
    update_edge_perm(&(cube->edges[e1]), BR);
    update_edge_perm(&(cube->edges[e2]), FL);
    
    e1 = which_edge_at_pos(FR, cube);
    e2 = which_edge_at_pos(BL, cube);
    update_edge_perm(&(cube->edges[e1]), BL);
    update_edge_perm(&(cube->edges[e2]), FR);
    
    e1 = which_corner_at_pos(UFL, cube);
    e2 = which_corner_at_pos(DBR, cube);
    update_corner_perm(&(cube->corners[e1]), DBR);
    update_corner_perm(&(cube->corners[e2]), UFL);
    
    e1 = which_corner_at_pos(UFR, cube);
    e2 = which_corner_at_pos(DBL, cube);
    update_corner_perm(&(cube->corners[e1]), DBL);
    update_corner_perm(&(cube->corners[e2]), UFR);
    
    e1 = which_corner_at_pos(UBL, cube);
    e2 = which_corner_at_pos(DFR, cube);
    update_corner_perm(&(cube->corners[e1]), DFR);
    update_corner_perm(&(cube->corners[e2]), UBL);
    
    e1 = which_corner_at_pos(UBR, cube);
    e2 = which_corner_at_pos(DFL, cube);
    update_corner_perm(&(cube->corners[e1]), DFL);
    update_corner_perm(&(cube->corners[e2]), UBR);
    
    for (int i = 0; i < NCORNERS; i++){
        update_corner_orien(&(cube->corners[i]),
            (3 - extract_corner_orien(cube->corners[i], FB)) % 3,
            (3 - extract_corner_orien(cube->corners[i], LR)) % 3,
            (3 - extract_corner_orien(cube->corners[i], UD)) % 3
        );
    }
}

const enum facemove {
  U, D,
  L, R,
  F, B,
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
