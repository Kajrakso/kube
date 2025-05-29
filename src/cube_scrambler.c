#include "../include/scramblers/scrambler.h"
#include "core/_cube.h"
#include "utils.h"

cube_t cube_scrambler_get_handscrambled_cube(){
    cube_t cube = cube_create_new_cube();

    // https://arxiv.org/pdf/2410.20630
    const int n_rand_moves = 26;
    for (int i = 0; i < n_rand_moves; i++){
        cube_move_apply_move(&cube, rand() % NMOVES);
    }

    return cube;
}

cube_t cube_scrambler_get_scrambled_cube(){
    int i, j;
    int c_parity, e_parity;
    int co_sum, eo_sum, rand_orien;

    cube_t cube = cube_create_new_cube();

    // random permutation of the pieces
    arr_shuffle(cube.corners, NCORNERS);
    arr_shuffle(cube.edges, NEDGES);

    c_parity = 0;
    for (i = 0; i < NCORNERS; i++){
        for (j = i + 1; j < NCORNERS; j++){
        if (extract_corner_perm(cube.corners[i]) > extract_corner_perm(cube.corners[j])) c_parity++;
        }
    }
    c_parity %= 2;

    e_parity = 0;
    for (i = 0; i < NEDGES; i++){
        for (j = i + 1; j < NEDGES; j++){
        if (extract_edge_perm(cube.edges[i]) > extract_edge_perm(cube.edges[j])) e_parity++;
        }
    }
    e_parity %= 2;

    // make sure the permutations results in a valid cube.
    if (c_parity != e_parity){
        swap_uint16(&cube.edges[0], &cube.edges[1]);
    }

    // generate random orientations for the FB axis.
    co_sum = 0;
    for (i = 0; i < NCORNERS - 1; i++){
        rand_orien = rand() % 3;
        update_corner_orien(&cube.corners[i], rand_orien, 0, 0);
        co_sum += rand_orien;
    }

    eo_sum = 0;
    for (i = 0; i < NEDGES - 1; i++){
        rand_orien = rand() % 2;
        update_edge_orien(&cube.edges[i], rand_orien, 0, 0);
        eo_sum += rand_orien;
    }

    // set the last corner and edge orientation so that the sum of all orientations results in a valid cube.
    update_corner_orien(&cube.corners[NCORNERS - 1], (3 - co_sum % 3) % 3, 0, 0);
    update_edge_orien(&cube.edges[NEDGES - 1], eo_sum % 2, 0, 0);

    // calculate the orientations along the LR and UD axes.
    fix_co_lr_ud(&cube);
    fix_eo_lr_ud(&cube);

    return cube;
}
