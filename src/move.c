#include "../include/core/move.h"
#include "_tables.h"
#include "_print_cube.h"

/* public */

void cube_move_apply_move(cube_t* cube, int move){
    uint16_t* p;
    
    p = move_table_corner_transformation[move];
    
    cube->corners[0] = p[cube->corners[0]];
    cube->corners[1] = p[cube->corners[1]];
    cube->corners[2] = p[cube->corners[2]];
    cube->corners[3] = p[cube->corners[3]];
    cube->corners[4] = p[cube->corners[4]];
    cube->corners[5] = p[cube->corners[5]];
    cube->corners[6] = p[cube->corners[6]];
    cube->corners[7] = p[cube->corners[7]];
    
    p = move_table_edge_transformation[move];

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

void cube_move_apply_multiple_moves(cube_t* cube, int* moves_arr, size_t number_of_moves){
    for (size_t i = 0; i < number_of_moves; i++){
        cube_move_apply_move(cube, moves_arr[i]);
    }
}

void cube_move_apply_move_string(cube_t* cube, const char* moves){
    size_t length;
    int* parsed_moves = parse_move_string(&length, moves);
    if (parsed_moves){
        cube_move_apply_multiple_moves(cube, parsed_moves, length);
        free(parsed_moves);
    }
}

/* private */

int
get_inv_move(int m){
    return m - 2 * (m % 3) + 2;
}

void cube_apply_symm(cube_t* cube, int sym){
    uint16_t* p;
    
    p = sym_table_corner_transformation[sym];
    
    cube->corners[0] = p[cube->corners[0]];
    cube->corners[1] = p[cube->corners[1]];
    cube->corners[2] = p[cube->corners[2]];
    cube->corners[3] = p[cube->corners[3]];
    cube->corners[4] = p[cube->corners[4]];
    cube->corners[5] = p[cube->corners[5]];
    cube->corners[6] = p[cube->corners[6]];
    cube->corners[7] = p[cube->corners[7]];
    
    p = sym_table_edge_transformation[sym];

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


