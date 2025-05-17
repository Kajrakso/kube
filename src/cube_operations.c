#include "_cube.h"
#include "utils.h"
#include "_tables.h"
#include "_sym.h"

bool cube_operation_is_equal(cube_t cube1, cube_t cube2){
    return arr_eq(cube1.edges, cube2.edges, NEDGES) && arr_eq(cube1.corners, cube2.corners, NCORNERS);
}

cube_t cube_operation_compose(cube_t cube1, cube_t cube2){
    cube_t result;
    
    for (int i = 0; i < NEDGES; i++){
      int e1 = cube1.edges[i];
      int i1 = extract_edge_perm(e1);
      
      int e2 = cube2.edges[i1];
      int i2 = extract_edge_perm(e2);

      result.edges[i] = build_edge(
        i2,
        extract_edge_orien(e1, FB) ^ extract_edge_orien(e2, FB),
        extract_edge_orien(e1, LR) ^ extract_edge_orien(e2, LR),
        extract_edge_orien(e1, UD) ^ extract_edge_orien(e2, UD)
      );
    }

    for (int i = 0; i < NCORNERS; i++){
      int c1 = cube1.corners[i];
      int i1 = extract_corner_perm(c1);

      int c2 = cube2.corners[i1];
      int i2 = extract_corner_perm(c2);

      result.corners[i] = build_corner(
        i2,
        (extract_corner_orien(c1, FB) + extract_corner_orien(c2, FB)) % 3,
        (extract_corner_orien(c1, LR) + extract_corner_orien(c2, LR)) % 3,
        (extract_corner_orien(c1, UD) + extract_corner_orien(c2, UD)) % 3
      );
    }

    return result;
}

cube_t cube_operation_inverse(cube_t cube){
    int i, eo_fb, eo_lr, eo_ud, co_fb, co_lr, co_ud;
    cube_t inverse;

    for(i = 0; i < NEDGES; i++){
        eo_fb = extract_edge_orien(cube.edges[i], FB);
        eo_lr = extract_edge_orien(cube.edges[i], LR);
        eo_ud = extract_edge_orien(cube.edges[i], UD);
        inverse.edges[extract_edge_perm(cube.edges[i])] = build_edge(i, eo_fb, eo_lr, eo_ud);
    }

    for(i = 0; i < NCORNERS; i++){
        co_fb = (3 - extract_corner_orien(cube.corners[i], FB)) % 3;
        co_lr = (3 - extract_corner_orien(cube.corners[i], LR)) % 3;
        co_ud = (3 - extract_corner_orien(cube.corners[i], UD)) % 3;
        inverse.corners[extract_corner_perm(cube.corners[i])] = build_corner(i, co_fb, co_lr, co_ud);
    }

    return inverse;
}

cube_t cube_operation_sym_conjugate(cube_t cube, int symmetry){
    cube_t new_cube = cube_create_new_cube();
    
    cube_apply_symm(&new_cube, symmetry);
    new_cube = cube_operation_compose(new_cube, cube);
    cube_apply_symm(&new_cube, get_inv_sym(symmetry));
    
    return new_cube;
}
