#include "../include/scramblers/scrambler.h"
#include "_cube.h"

// should rewrite to actually draw a random cube
cube_t cube_scrambler_get_scrambled_cube(){
    cube_t cube = cube_create_new_cube();

    // https://arxiv.org/pdf/2410.20630
    const int n_rand_moves = 26;
    for (int i = 0; i < n_rand_moves; i++){
        cube_apply_move(&cube, rand() % NMOVES);
    }
}
