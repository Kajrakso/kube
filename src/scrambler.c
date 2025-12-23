#include <string.h>

#include "core/cube.h"
#include "core/move.h"
#include "utils.h"
#include "tables.h"

cube_t cube_scrambler_get_handscrambled_cube() {
    cube_t cube = cube_create_new_cube();

    // https://arxiv.org/pdf/2410.20630
    const int n_rand_moves = 26;
    for (int i = 0; i < n_rand_moves; i++)
    {
        cube_move_apply_move(&cube, rand() % NMOVES);
    }

    return cube;
}

cube_t cube_scrambler_get_scrambled_cube() {
    int i, j;
    int c_parity, e_parity;
    int co_sum, eo_sum, rand_orien;

    cube_t cube = cube_create_new_cube();

    // random permutation of the pieces
    arr_shuffle(cube.corners, NCORNERS);
    arr_shuffle(cube.edges, NEDGES);

    c_parity = 0;
    for (i = 0; i < NCORNERS; i++)
    {
        for (j = i + 1; j < NCORNERS; j++)
        {
            if (extract_corner_perm(cube.corners[i]) > extract_corner_perm(cube.corners[j]))
                c_parity++;
        }
    }
    c_parity %= 2;

    e_parity = 0;
    for (i = 0; i < NEDGES; i++)
    {
        for (j = i + 1; j < NEDGES; j++)
        {
            if (extract_edge_perm(cube.edges[i]) > extract_edge_perm(cube.edges[j]))
                e_parity++;
        }
    }
    e_parity %= 2;

    // make sure the permutations results in a valid cube.
    if (c_parity != e_parity)
    {
        swap_uint16(&cube.edges[0], &cube.edges[1]);
    }

    // generate random orientations for the FB axis.
    co_sum = 0;
    for (i = 0; i < NCORNERS - 1; i++)
    {
        rand_orien = rand() % 3;
        update_corner_orien(&cube.corners[i], rand_orien, 0, 0);
        co_sum += rand_orien;
    }

    eo_sum = 0;
    for (i = 0; i < NEDGES - 1; i++)
    {
        rand_orien = rand() % 2;
        update_edge_orien(&cube.edges[i], rand_orien, 0, 0);
        eo_sum += rand_orien;
    }

    // set the last corner and edge orientation so that the sum of all
    // orientations results in a valid cube.
    update_corner_orien(&cube.corners[NCORNERS - 1], (3 - co_sum % 3) % 3, 0, 0);
    update_edge_orien(&cube.edges[NEDGES - 1], eo_sum % 2, 0, 0);

    // calculate the orientations along the LR and UD axes.
    fix_co_lr_ud(&cube);
    fix_eo_lr_ud(&cube);

    return cube;
}





cube_t cube_scrambler_get_scrambled_DR(){
    int i, j;
    int c_parity, e_parity;

    cube_t cube = cube_create_new_cube();

    // random permutation of the pieces
    arr_shuffle(cube.corners, NCORNERS);
    arr_shuffle(&cube.edges[0], 4);
    arr_shuffle(&cube.edges[4], 4);
    arr_shuffle(&cube.edges[8], 4);

    c_parity = 0;
    for (i = 0; i < NCORNERS; i++)
    {
        for (j = i + 1; j < NCORNERS; j++)
        {
            if (extract_corner_perm(cube.corners[i]) > extract_corner_perm(cube.corners[j]))
                c_parity++;
        }
    }
    c_parity %= 2;

    e_parity = 0;
    for (i = 0; i < NEDGES; i++)
    {
        for (j = i + 1; j < NEDGES; j++)
        {
            if (extract_edge_perm(cube.edges[i]) > extract_edge_perm(cube.edges[j]))
                e_parity++;
        }
    }
    e_parity %= 2;

    // make sure the permutations results in a valid cube.
    if (c_parity != e_parity)
    {
        swap_uint16(&cube.edges[0], &cube.edges[1]);
    }

    // calculate the orientations along the LR and UD axes.
    fix_co_lr_fb(&cube);
    fix_eo_lr_ud(&cube);

    return cube;
}











int cube_scrambler_parse_speffz(cube_t* c, char* s) {
    int letter_syms[24] = {2, 3, 0, 1, 13, 14, 15, 12, 11, 8,  9,  10,
                           4, 5, 6, 7, 22, 23, 20, 21, 17, 18, 19, 16};

    int sticker_to_piece_c[24] = {UBL, UBR, UFR, UFL, UBL, UFL, DFL, DBL, UFL, UFR, DFR, DFL,
                                  UFR, UBR, DBR, DFR, UBR, UBL, DBL, DBR, DFL, DFR, DBR, DBL};
    int sticker_to_piece_e[24] = {UB, UR, UF, UL, UL, FL, DL, BL, UF, FR, DF, FL,
                                  UR, BR, DR, FR, UB, BL, DB, BR, DF, DR, DB, DL};

    int buf_c = UFR;
    int buf_e = UF;

    int  parity      = 0;
    bool parse_edges = false;
    for (int i = 0; i < strlen(s); i++)
    {
        if (s[i] == '.')
        {
            parse_edges = true;
        }
        else
        {
            int target = s[i] - 'a';

            if (0 <= target && target <= 24)
            {

                if (!parse_edges && target != buf_c)
                {
                    int c1 = which_corner_at_pos(buf_c, c);
                    int c2 = which_corner_at_pos(sticker_to_piece_c[target], c);

                    int sym1 = letter_syms[target];
                    int sym2 = get_inv_sym(sym1);

                    // swap c1 and c2 by
                    // using precomputed sym_tables
                    c->corners[c1] = sym_table_corner_transformation[sym1][c->corners[c1]];
                    c->corners[c2] = sym_table_corner_transformation[sym2][c->corners[c2]];

                    parity ^= 1;
                }
                else if (target != buf_e)
                {
                    int e1 = which_edge_at_pos(buf_e, c);
                    int e2 = which_edge_at_pos(sticker_to_piece_e[target], c);

                    int sym1 = letter_syms[target];
                    int sym2 = get_inv_sym(sym1);

                    // swap e1 and e2 by
                    // using precomputed sym_tables
                    c->edges[e1] = sym_table_edge_transformation[sym1][c->edges[e1]];
                    c->edges[e2] = sym_table_edge_transformation[sym2][c->edges[e2]];

                    parity ^= 1;
                }
            }
        }
    }
    if (parity == 0)
    {
        *c = cube_operation_inverse(*c);
        return 0;
    }
    else
    {
        return 1;
    }
}
