#include "_cube.h"
#include "utils.h"

bool cube_operation_is_equal(cube_t cube1, cube_t cube2){
    return arr_eq(cube1.edges, cube2.edges, NEDGES) && arr_eq(cube1.corners, cube2.corners, NCORNERS);
}

cube_t cube_operation_compose(cube_t cube1, cube_t cube2){
    // todo
    // this function should return the composition of applying cube1 and cube2 in the Rubik's cube group.
    return cube_create_new_cube();
}

cube_t cube_operation_inverse(cube_t cube){
    cube_t inverse = cube_create_new_cube();

    for(int i = 0; i < NEDGES; i++){
        int eo_fb = extract_edge_orien(cube.edges[i], FB);
        int eo_lr = extract_edge_orien(cube.edges[i], LR);
        int eo_ud = extract_edge_orien(cube.edges[i], UD);
        inverse.edges[extract_edge_perm(cube.edges[i])] = build_edge(i, eo_fb, eo_lr, eo_ud);
    }

    for(int i = 0; i < NCORNERS; i++){
        int co_fb = (3 - extract_corner_orien(cube.corners[i], FB)) % 3;
        int co_lr = (3 - extract_corner_orien(cube.corners[i], LR)) % 3;
        int co_ud = (3 - extract_corner_orien(cube.corners[i], UD)) % 3;
        inverse.corners[extract_corner_perm(cube.corners[i])] = build_corner(i, co_fb, co_lr, co_ud);
    }

    return inverse;
}
