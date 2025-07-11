#include "../tables.h"

/* the symmetry at index i is i^(-1). */
static const int sym_inv[NSYMS] = {0,  3,  2,  1,  8,  5,  23, 14, 4,  22, 13, 11, 12, 10, 7,  21,
                                   16, 17, 18, 19, 20, 15, 9,  6,  24, 27, 26, 25, 32, 29, 47, 38,
                                   28, 46, 37, 35, 36, 34, 31, 45, 40, 41, 42, 43, 44, 39, 33, 30};

int get_inv_sym(int s) { return sym_inv[s]; }

void cube_apply_symm(cube_t* cube, int sym) {
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

    cube->edges[0]  = p[cube->edges[0]];
    cube->edges[1]  = p[cube->edges[1]];
    cube->edges[2]  = p[cube->edges[2]];
    cube->edges[3]  = p[cube->edges[3]];
    cube->edges[4]  = p[cube->edges[4]];
    cube->edges[5]  = p[cube->edges[5]];
    cube->edges[6]  = p[cube->edges[6]];
    cube->edges[7]  = p[cube->edges[7]];
    cube->edges[8]  = p[cube->edges[8]];
    cube->edges[9]  = p[cube->edges[9]];
    cube->edges[10] = p[cube->edges[10]];
    cube->edges[11] = p[cube->edges[11]];
}

void do_y_rot(cube_t* cube) {
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

void do_z_rot(cube_t* cube) {
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

void do_inversion(cube_t* cube) {
    int e1, e2;

    // build a "reverse" lookup for corners
    int pos_to_corner[NCORNERS];
    build_pos_to_corner(cube, pos_to_corner, UD);

    // build a "reverse" lookup for edges
    int pos_to_edge[NEDGES];
    build_pos_to_edge(cube, pos_to_edge, UD);

    e1 = pos_to_edge[UF];
    e2 = pos_to_edge[DB];
    update_edge_perm(&(cube->edges[e1]), DB);
    update_edge_perm(&(cube->edges[e2]), UF);

    e1 = pos_to_edge[UL];
    e2 = pos_to_edge[DR];
    update_edge_perm(&(cube->edges[e1]), DR);
    update_edge_perm(&(cube->edges[e2]), UL);

    e1 = pos_to_edge[UR];
    e2 = pos_to_edge[DL];
    update_edge_perm(&(cube->edges[e1]), DL);
    update_edge_perm(&(cube->edges[e2]), UR);

    e1 = pos_to_edge[UB];
    e2 = pos_to_edge[DF];
    update_edge_perm(&(cube->edges[e1]), DF);
    update_edge_perm(&(cube->edges[e2]), UB);

    e1 = pos_to_edge[FL];
    e2 = pos_to_edge[BR];
    update_edge_perm(&(cube->edges[e1]), BR);
    update_edge_perm(&(cube->edges[e2]), FL);

    e1 = pos_to_edge[FR];
    e2 = pos_to_edge[BL];
    update_edge_perm(&(cube->edges[e1]), BL);
    update_edge_perm(&(cube->edges[e2]), FR);

    e1 = pos_to_corner[UFL];
    e2 = pos_to_corner[DBR];
    update_corner_perm(&(cube->corners[e1]), DBR);
    update_corner_perm(&(cube->corners[e2]), UFL);

    e1 = pos_to_corner[UFR];
    e2 = pos_to_corner[DBL];
    update_corner_perm(&(cube->corners[e1]), DBL);
    update_corner_perm(&(cube->corners[e2]), UFR);

    e1 = pos_to_corner[UBL];
    e2 = pos_to_corner[DFR];
    update_corner_perm(&(cube->corners[e1]), DFR);
    update_corner_perm(&(cube->corners[e2]), UBL);

    e1 = pos_to_corner[UBR];
    e2 = pos_to_corner[DFL];
    update_corner_perm(&(cube->corners[e1]), DFL);
    update_corner_perm(&(cube->corners[e2]), UBR);

    for (int i = 0; i < NCORNERS; i++)
    {
        update_corner_orien(&(cube->corners[i]),
                            (3 - extract_corner_orien(cube->corners[i], FB)) % 3,
                            (3 - extract_corner_orien(cube->corners[i], LR)) % 3,
                            (3 - extract_corner_orien(cube->corners[i], UD)) % 3);
    }
}
