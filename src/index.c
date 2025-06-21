#include "index.h"

int ud_i_to_fb_i_corners[NCORNERS] = {4, 7, 0, 3, 2, 1, 6, 5};
int ud_i_to_lr_i_corners[NCORNERS] = {2, 5, 4, 3, 0, 7, 6, 1};
int ud_i_to_fb_i_edges[NEDGES]     = {7, 8, 3, 11, 1, 9, 5, 10, 4, 6, 0, 2};
int ud_i_to_lr_i_edges[NEDGES]     = {10, 4, 11, 2, 8, 6, 9, 0, 1, 5, 7, 3};

/* 0, ..., 70 - 1 = comb(8, 4) - 1 */
uint64_t cube_to_cc_index(cube_t* cube, axes ax) {
    if (ax == UD)
    {
        return (uint64_t)
          ccu_combinatorials_lookup[extract_corner_perm(cube->corners[UBL]) * 1
                                    + extract_corner_perm(cube->corners[UBR]) * 8
                                    + extract_corner_perm(cube->corners[UFR]) * 8 * 8
                                    + extract_corner_perm(cube->corners[UFL]) * 8 * 8 * 8];
    }
    if (ax == FB)
    {
        return (uint64_t) ccu_combinatorials_lookup
          [ud_i_to_fb_i_corners[extract_corner_perm(cube->corners[UFL])] * 1
           + ud_i_to_fb_i_corners[extract_corner_perm(cube->corners[UFR])] * 8
           + ud_i_to_fb_i_corners[extract_corner_perm(cube->corners[DFR])] * 8 * 8
           + ud_i_to_fb_i_corners[extract_corner_perm(cube->corners[DFL])] * 8 * 8 * 8];
    }
    if (ax == LR)
    {
        return (uint64_t) ccu_combinatorials_lookup
          [ud_i_to_lr_i_corners[extract_corner_perm(cube->corners[DFL])] * 1
           + ud_i_to_lr_i_corners[extract_corner_perm(cube->corners[DBL])] * 8
           + ud_i_to_lr_i_corners[extract_corner_perm(cube->corners[UBL])] * 8 * 8
           + ud_i_to_lr_i_corners[extract_corner_perm(cube->corners[UFL])] * 8 * 8 * 8];
    }

    return 0;
}

/* 0, .., 495 - 1 = comb(12, 4) - 1 */
uint64_t cube_to_ec_index(cube_t* cube, axes ax) {
    if (ax == UD)
    {
        return (
          uint64_t) ece_combinatorials_lookup[extract_edge_perm(cube->edges[FR]) * 1
                                              + extract_edge_perm(cube->edges[FL]) * 12
                                              + extract_edge_perm(cube->edges[BL]) * 12 * 12
                                              + extract_edge_perm(cube->edges[BR]) * 12 * 12 * 12];
    }
    if (ax == FB)
    {
        return (uint64_t) ece_combinatorials_lookup
          [ud_i_to_fb_i_edges[extract_edge_perm(cube->edges[UL])] * 1
           + ud_i_to_fb_i_edges[extract_edge_perm(cube->edges[UR])] * 12
           + ud_i_to_fb_i_edges[extract_edge_perm(cube->edges[DR])] * 12 * 12
           + ud_i_to_fb_i_edges[extract_edge_perm(cube->edges[DL])] * 12 * 12 * 12];
    }
    if (ax == LR)
    {
        return (uint64_t) ece_combinatorials_lookup
          [ud_i_to_lr_i_edges[extract_edge_perm(cube->edges[UB])] * 1
           + ud_i_to_lr_i_edges[extract_edge_perm(cube->edges[UF])] * 12
           + ud_i_to_lr_i_edges[extract_edge_perm(cube->edges[DF])] * 12 * 12
           + ud_i_to_lr_i_edges[extract_edge_perm(cube->edges[DB])] * 12 * 12 * 12];
    }
    return 0;
}

/* 0, ..., 2**11 - 1 */
uint64_t cube_to_eo_index(cube_t* cube, axes ax) {
    // build a "reverse" lookup
    int pos_to_edge[NEDGES];
    switch (ax)
    {
    case FB :
        build_pos_to_edge(cube, pos_to_edge, UD);
        break;
    case UD :
        build_pos_to_edge(cube, pos_to_edge, LR);
        break;
    case LR :
        build_pos_to_edge(cube, pos_to_edge, FB);
        break;
    }

    uint64_t orien = 0;
    uint64_t pow   = 1ULL;
    for (int i = 0; i < NEDGES - 1; i++)
    {
        orien += pow * (uint64_t) extract_edge_orien(cube->edges[pos_to_edge[i]], ax);
        pow <<= 1;
    }
    return orien;
}

/* 0, ..., 3**7 - 1 */
uint64_t cube_to_co_index(cube_t* cube, axes ax) {
    // build a "reverse" lookup
    int pos_to_corner[NCORNERS];
    build_pos_to_corner(cube, pos_to_corner, ax);

    uint64_t     orien = 0;
    unsigned int pow   = 1;
    for (int i = 0; i < NCORNERS - 1; i++)
    {
        orien += pow * (uint64_t) extract_corner_orien(cube->corners[pos_to_corner[i]], ax);
        pow *= 3;
    }
    return orien;
}

/* 0, ..., 153090 - 1 */
uint64_t cube_to_c_index(cube_t* cube, axes ax) {
    return cc_co_to_c_index(cube_to_cc_index(cube, ax), cube_to_co_index(cube, ax));
}

/* 0, ..., 495*2**11 - 1 = 1013760 - 1 */
uint64_t cube_to_e_index(cube_t* cube, axes ax) {
    switch (ax)
    {
    case UD :
        return ec_eo_to_e_index(cube_to_ec_index(cube, UD), cube_to_eo_index(cube, FB));
    case FB :
        return ec_eo_to_e_index(cube_to_ec_index(cube, FB), cube_to_eo_index(cube, LR));
    case LR :
        return ec_eo_to_e_index(cube_to_ec_index(cube, LR), cube_to_eo_index(cube, UD));
    }
}

inline uint64_t cc_co_to_c_index(uint64_t cc, uint64_t co) { return cc + NCCU * (co); }

inline uint64_t ec_eo_to_e_index(uint64_t ec, uint64_t eo) { return ec + NECE * (eo); }

uint64_t cclass_i_e_to_H_index(uint64_t cclass_i, uint64_t e_i) { return cclass_i + NCCLASS * e_i; }


uint64_t cube_to_H_index(cube_t* cube, axes ax) {
    uint64_t*                 sym_table_e_index = get_sym_table_e_index();
    uint64_t                  c_i, e_i;    // before sym red.
    uint64_t                  c_i2, e_i2;  // after sym red;
    struct c_index_cclass_sym c;

    switch (ax)
    {
    case UD :
        c_i = cc_co_to_c_index(cube_to_cc_index(cube, UD), cube_to_co_index(cube, UD));
        e_i = ec_eo_to_e_index(cube_to_ec_index(cube, UD), cube_to_eo_index(cube, FB));
        break;
    case LR :
        c_i = cc_co_to_c_index(cube_to_cc_index(cube, LR), cube_to_co_index(cube, LR));
        e_i = ec_eo_to_e_index(cube_to_ec_index(cube, LR), cube_to_eo_index(cube, UD));
        break;
    case FB :
        c_i = cc_co_to_c_index(cube_to_cc_index(cube, FB), cube_to_co_index(cube, FB));
        e_i = ec_eo_to_e_index(cube_to_ec_index(cube, FB), cube_to_eo_index(cube, LR));
        break;
    }

    c    = cclass_table[c_i];
    e_i2 = sym_table_e_index[e_i * NSYMS + c.sym];

    c_i2 = c.cclass_i;
    return cclass_i_e_to_H_index(c_i2, e_i2);
}


/*
  TODO: remove?
  TODO: rewrite/refactor/delete!
  TODO: need testing. well, all of these functions need testing.
*/
cube_t ccu_index_to_cube(uint64_t ccu_i) {
    cube_t cube = cube_create_new_cube();

    // set ccu
    // bad code. need to fix
    int m = -1;
    for (int k = 0; k < 4096; k++)
    {
        if (ccu_combinatorials_lookup[k] == ccu_i)
        {
            m = k;
            break;
        }
    }
    if (m == -1)
    {
        // OH NO!
        exit(1);
    }
    int UBL_i = m % 8;
    int UBR_i = (m % (8 * 8)) / 8;
    int UFR_i = (m % (8 * 8 * 8)) / (8 * 8);
    int UFL_i = (m / (8 * 8 * 8));

    // todo: ugly, but it works
    int e  = which_corner_at_pos(UBL_i, &cube);
    int ep = extract_corner_perm(cube.corners[UBL]);
    update_corner_perm(&cube.corners[UBL], UBL_i);
    update_corner_perm(&cube.corners[e], ep);

    e  = which_corner_at_pos(UBR_i, &cube);
    ep = extract_corner_perm(cube.corners[UBR]);
    update_corner_perm(&cube.corners[UBR], UBR_i);
    update_corner_perm(&cube.corners[e], ep);

    e  = which_corner_at_pos(UFR_i, &cube);
    ep = extract_corner_perm(cube.corners[UFR]);
    update_corner_perm(&cube.corners[UFR], UFR_i);
    update_corner_perm(&cube.corners[e], ep);

    e  = which_corner_at_pos(UFL_i, &cube);
    ep = extract_corner_perm(cube.corners[UFL]);
    update_corner_perm(&cube.corners[UFL], UFL_i);
    update_corner_perm(&cube.corners[e], ep);

    fix_eo_lr_ud(&cube);
    return cube;
}

cube_t coud_index_to_cube(uint64_t coud_i) {
    cube_t cube = cube_create_new_cube();

    int c_i, co, co_sum = 0, pow = 1;

    for (int i = 0; i < NCORNERS - 1; i++)
    {
        co  = (coud_i / pow) % 3;
        c_i = which_corner_at_pos(i, &cube);

        update_corner_orien(&cube.corners[c_i], co, 0, 0);
        co_sum = co_sum + co;

        pow *= 3;
    }

    c_i = which_corner_at_pos(NCORNERS - 1, &cube);
    update_corner_orien(&cube.corners[c_i], (3 - co_sum % 3) % 3, 0, 0);

    fix_co_lr_ud(&cube);
    return cube;
}

cube_t c_index_to_cube(uint64_t c_i) {
    cube_t cube1 = ccu_index_to_cube(c_i % NCCU);
    cube_t cube2 = coud_index_to_cube(c_i / NCCU);

    return cube_operation_compose(cube1, cube2);
}
