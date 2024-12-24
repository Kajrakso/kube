#include "../include/scramblers/scrambler.h"
#include "_cube.h"

// should rewrite to actually draw a random cube
cube_t cube_scrambler_get_scrambled_cube(){
    cube_t cube = cube_create_new_cube();

    // https://arxiv.org/pdf/2410.20630
    const int n_rand_moves = 26;

    int moves[n_rand_moves];
    for (int i = 0; i < n_rand_moves; i++){
        moves[i] = rand() % NMOVES;
    }

    for (int i = 0; i < 26; i++){
        cube_apply_move(&cube, moves[i]);
    }
}
