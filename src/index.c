#include "index.h"

/* The axis are defined by "swivelling" around the UFL-DBR diagonal, starting with U up and F front. */

int i_transform_axes_c[NAXES][NCORNERS] = {[FB] = {4, 7, 0, 3, 2, 1, 6, 5},
                                           [LR] = {2, 5, 4, 3, 0, 7, 6, 1},
                                           [UD] = {0, 1, 2, 3, 4, 5, 6, 7}};

int i_transform_axes_e[NAXES][NEDGES] = {[FB] = {7, 8, 3, 11, 1, 9, 5, 10, 4, 6, 0, 2},
                                         [LR] = {10, 4, 11, 2, 8, 6, 9, 0, 1, 5, 7, 3},
                                         [UD] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}};

static int cs[NAXES][4] = {
  [FB] = {UFL, UFR, DFR, DFL}, [LR] = {DFL, DBL, UBL, UFL}, [UD] = {UBL, UBR, UFR, UFL}};

/* 0, ..., 70 - 1 = comb(8, 4) - 1 */
uint64_t cube_to_cc_index(cube_t* cube, axes ax) {
    return ccu_combinatorials_lookup
      [i_transform_axes_c[ax][extract_corner_perm(cube->corners[cs[ax][0]])] * 1
       + i_transform_axes_c[ax][extract_corner_perm(cube->corners[cs[ax][1]])] * 8
       + i_transform_axes_c[ax][extract_corner_perm(cube->corners[cs[ax][2]])] * 8 * 8
       + i_transform_axes_c[ax][extract_corner_perm(cube->corners[cs[ax][3]])] * 8 * 8 * 8];
}

/* 0, .., 495 - 1 = comb(12, 4) - 1 */
uint64_t cube_to_ec_index(cube_t* cube, axes ax) {
    if (ax == UD)
    {
        return ece_combinatorials_lookup[extract_edge_perm(cube->edges[FR]) * 1
                                              + extract_edge_perm(cube->edges[FL]) * 12
                                              + extract_edge_perm(cube->edges[BL]) * 12 * 12
                                              + extract_edge_perm(cube->edges[BR]) * 12 * 12 * 12];
    }
    if (ax == FB)
    {
        return ece_combinatorials_lookup
          [i_transform_axes_e[ax][extract_edge_perm(cube->edges[UL])] * 1
           + i_transform_axes_e[ax][extract_edge_perm(cube->edges[UR])] * 12
           + i_transform_axes_e[ax][extract_edge_perm(cube->edges[DR])] * 12 * 12
           + i_transform_axes_e[ax][extract_edge_perm(cube->edges[DL])] * 12 * 12 * 12];
    }
    if (ax == LR)
    {
        return ece_combinatorials_lookup
          [i_transform_axes_e[ax][extract_edge_perm(cube->edges[UB])] * 1
           + i_transform_axes_e[ax][extract_edge_perm(cube->edges[UF])] * 12
           + i_transform_axes_e[ax][extract_edge_perm(cube->edges[DF])] * 12 * 12
           + i_transform_axes_e[ax][extract_edge_perm(cube->edges[DB])] * 12 * 12 * 12];
    }
    return 0;
}

/* 0, ..., 2**11 - 1 */
uint64_t cube_to_eo_index(cube_t* cube, axes ax) {
    // build a "reverse" lookup
    int pos_to_edge[NEDGES];
    build_pos_to_edge(cube, pos_to_edge, ax);

    uint64_t orien = 0;
    uint64_t pow   = 1ULL;
    for (int i = 0; i < NEDGES - 1; i++)
    {
        /* the (ax + 1) % 3 trick is 
         * to adjust for EO axis being different
         * than the axis for DR. This is a result of
         * how EO is defined. */
        orien += pow * (uint64_t) extract_edge_orien(cube->edges[pos_to_edge[i]], (ax + 1) % 3);
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
    return ec_eo_to_e_index(cube_to_ec_index(cube, ax), cube_to_eo_index(cube, ax));
}

inline uint64_t cc_co_to_c_index(uint64_t cc, uint64_t co) { return cc + NCCU * (co); }

inline uint64_t ec_eo_to_e_index(uint64_t ec, uint64_t eo) { return ec + NECE * (eo); }

uint64_t cclass_i_e_to_H_index(uint64_t cclass_i, uint64_t e_i) { return cclass_i + NCCLASS * e_i; }


uint64_t cube_to_H_index(cube_t* cube, axes ax) {
    uint64_t*                 sym_table_e_index = get_sym_table_e_index();
    uint64_t                  c_i, e_i;    // before sym red.
    uint64_t                  c_i2, e_i2;  // after sym red.
    struct c_index_cclass_sym c;

    c_i = cc_co_to_c_index(cube_to_cc_index(cube, ax), cube_to_co_index(cube, ax));
    e_i = ec_eo_to_e_index(cube_to_ec_index(cube, ax), cube_to_eo_index(cube, ax));

    c    = cclass_table[c_i];
    e_i2 = sym_table_e_index[e_i * NSYMS + c.sym];

    c_i2 = c.cclass_i;
    return cclass_i_e_to_H_index(c_i2, e_i2);
}


uint64_t cube_to_DR_index(cube_t* cube, axes ax) {
    uint64_t                  co_i, eo_i, ec_i;    // before sym red.

    co_i = cube_to_co_index(cube, ax);
    eo_i = cube_to_eo_index(cube, ax);
    ec_i = cube_to_ec_index(cube, ax);

    return co_i + NCO * (eo_i + NEO * ec_i);
}


/* Partial indeces */
/* These are used to construct tables for n solved edges/corners */
/* The cp/ep index could be computed along different axis, but I am unsure is this is acutally useful for
 * solving to partially solved cubes.
 * */


/* 0, ..., ? - 1 = ? - 1 */
/* compute the combinatorial partial cp index for n corners. */
uint64_t cube_to_partial_cpc_index(cube_t* cube, axes ax, int* corners, uint64_t n) {
    int pos[NCORNERS];
    int count = 0;
    for (int i = 0; i < NCORNERS; i++) {
        for (uint64_t j = 0; j < n; j++) {
            if (corners[j] == i) {
                pos[count++] = i_transform_axes_c[ax][extract_corner_perm(cube->corners[i])];
                break;
            }
        }
    }

    for (uint64_t i = 1; i < n; i++) {
        int key = pos[i], j = (int)i - 1;
        while (j >= 0 && pos[j] > key) {
            pos[j + 1] = pos[j];
            j--;
        }
        pos[j + 1] = key;
    }

    uint64_t idx = 0;
    for (uint64_t i = 0; i < n; i++) {
        idx += (uint64_t)comb(pos[i], (int)i + 1);
    }

    return idx;
}



/* 0, ..., ? - 1 = ? - 1 */
/* compute the permutational partial cp index for n corners. */
uint64_t cube_to_partial_cpp_index(cube_t* cube, axes ax, int* corners, uint64_t n) {
    int p[4], idx = 0;
    for (int i = 0; i < NCORNERS; i++) {
        for (uint64_t j = 0; j < n; j++){
            if (corners[j] == i) {
                p[idx++] = i_transform_axes_c[ax][extract_corner_perm(cube->corners[i])];
                break;
            }
        }
    }

    int perm[4];
    for (uint64_t i = 0; i < n; i++) {
        perm[i] = 0;
        for (uint64_t j = 0; j < n; j++) {
            if (j != i && p[j] < p[i]){
                perm[i]++;
            }
        }
    }

    return perm_to_fact(perm, (int)n);
}

/* 0, ..., ? - 1 = ? - 1 */
/* compute the partial co index for n corners. */
uint64_t cube_to_partial_co_index(cube_t* cube, axes ax, int* corners, uint64_t n) {
    int pos_to_corner[NCORNERS];
    build_pos_to_corner(cube, pos_to_corner, ax);

    uint64_t     orien = 0;
    unsigned int pow   = 1;
    for (int i = 0; i < NCORNERS; i++) {
        int c = pos_to_corner[i];
        for (uint64_t j = 0; j < n; j++){
            if (corners[j] == c) {
                orien += pow * (uint64_t) extract_corner_orien(cube->corners[c], ax);
                pow *= 3;
                break;
            }
        }
    }

    return orien;
}

/* 0, ..., ? - 1 = ? - 1 */
/* compute the combinatorial partial ep index for n edges. */
uint64_t cube_to_partial_epc_index(cube_t* cube, axes ax, int* edges, uint64_t n) {
    int pos[NEDGES];
    int count = 0;

    for (int i = 0; i < NEDGES; i++) {
        for (uint64_t j = 0; j < n; j++) {
            if (edges[j] == i) {
                pos[count++] = i_transform_axes_e[ax][extract_edge_perm(cube->edges[i])];
                break;
            }
        }
    }

    /* insertion sort */
    for (uint64_t i = 1; i < n; i++) {
        int key = pos[i], j = (int)i - 1;
        while (j >= 0 && pos[j] > key) { pos[j + 1] = pos[j]; j--; }
        pos[j + 1] = key;
    }

    /* combinatorial number system index */
    uint64_t idx = 0;
    for (uint64_t i = 0; i < n; i++) {
        idx += (uint64_t)comb(pos[i], (int)i + 1);
    }

    return idx;
}


/* 0, ..., ? - 1 = ? - 1 */
/* compute the permutational partial ep index for n corners. */
uint64_t cube_to_partial_epp_index(cube_t* cube, axes ax, int* edges, uint64_t n) {
    // here I need to make a permutation of numbers {0, ..., 3} that gives the
    // permutation of the edges in int* edges. Note that the int* edges can be in any order,
    // so the permutation is how to permute the *ordered* list of int* edges in order to 
    // obtain the permutation they are on the cube.

    int p[4], idx = 0;
    for (int i = 0; i < NEDGES; i++) {
        for (uint64_t j = 0; j < n; j++){
            if (edges[j] == i) {
                p[idx++] = i_transform_axes_e[ax][extract_edge_perm(cube->edges[i])];
                break;
            }
        }
    }

    int perm[4];
    for (uint64_t i = 0; i < n; i++) {
        perm[i] = 0;
        for (uint64_t j = 0; j < n; j++) {
            if (j != i && p[j] < p[i]) {
                perm[i]++;
            }
        }
    }

    return perm_to_fact(perm, (int)n);
}

/* 0, ..., ? - 1 = ? - 1 */
/* compute the partial eo index for n edges. */
uint64_t cube_to_partial_eo_index(cube_t* cube, axes ax, int* edges, uint64_t n) {
    // build a "reverse" lookup
    int pos_to_edge[NEDGES];
    build_pos_to_edge(cube, pos_to_edge, ax);

    uint64_t orien = 0;
    uint64_t pow   = 1ULL;
    for (int i = 0; i < NEDGES; i++) {
        int e = pos_to_edge[i];
        for (uint64_t j = 0; j < n; j++){
            if (edges[j] == e) {
                orien += pow * (uint64_t) extract_edge_orien(cube->edges[e], ax);
                pow <<= 1;
                break;
            }
        }
    }

    return orien;
}

inline uint64_t partial_eofb_epc_epp_to_partial_e_index(uint64_t partial_eo, uint64_t partial_epc, uint64_t partial_epp, uint64_t n){
    return partial_eo
        + ipow64(2, n) * (partial_epc
        + comb_u64(NEDGES, n) * (partial_epp
    ));
}

/* 0, ..., ? - 1 = ? - 1 */
/* compute the partial e index for n edges. */
uint64_t cube_to_partial_e_index(cube_t* cube, axes ax, int* edges, uint64_t n) {
    return partial_eofb_epc_epp_to_partial_e_index(
        cube_to_partial_eo_index(cube, ax, edges, n),
        cube_to_partial_epc_index(cube, ax, edges, n),
        cube_to_partial_epp_index(cube, ax, edges, n),
        n
    );
}


inline uint64_t partial_coud_cpc_cpp_to_partial_c_index(uint64_t partial_co, uint64_t partial_cpc, uint64_t partial_cpp, uint64_t n){
    return partial_co
        + ipow64(3, n) * (partial_cpc
        + comb_u64(NCORNERS, n) * (partial_cpp
    ));
}

/* 0, ..., ? - 1 = ? - 1 */
/* compute the partial c index for n corners. */
uint64_t cube_to_partial_c_index(cube_t* cube, axes ax, int* corners, uint64_t n) {
    return partial_coud_cpc_cpp_to_partial_c_index(
        cube_to_partial_co_index(cube, ax, corners, n),
        cube_to_partial_cpc_index(cube, ax, corners, n),
        cube_to_partial_cpp_index(cube, ax, corners, n),
        n
    );
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

    int c_i, co, co_sum = 0;
    uint64_t pow = 1;

    for (int i = 0; i < NCORNERS - 1; i++)
    {
        co  = (int)((coud_i / pow) % 3);
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


