#include "_move_tables.h"

uint16_t move_table_corner_transformation[NMOVES][NCORNERCUBIES];
uint16_t move_table_edge_transformation[NMOVES][NEDGECUBIES];

enum facemove {
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

/* Fills the tables with default values.
This have to be done before we generate the
actual moves. */
void initialize_move_tables(){
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


void generate_move_tables(){
    initialize_move_tables();
    gen_move_tables();
}


bool save_move_tables(const char *filename) {
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

    fclose(file);
    return true;
}

bool load_move_tables(const char *filename) {
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

    fclose(file);
    return true;
}
