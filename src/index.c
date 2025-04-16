#include "_index.h"

/* TODO: reconsider the way to calculate these indeces. It need to be fast. */

static int which_corner_at_pos(int pos, cube_t* cube);
static int which_edge_at_pos(int pos, cube_t* cube);

/* 0, .., 495 - 1 = comb(12, 4) - 1 */
int
cube_to_ece_index(cube_t* cube){
    return combinatorials_lookup[
          extract_edge_perm(cube->edges[FR]) * 1
        + extract_edge_perm(cube->edges[FL]) * 12
        + extract_edge_perm(cube->edges[BL]) * 144
        + extract_edge_perm(cube->edges[BR]) * 1728
    ];
}

/* 0, ..., 2**11 - 1 */
int
cube_to_eofb_index(cube_t* cube){
    int orien = 0;
    int pow = 1;
    for (int i = 0; i < NEDGES - 1; i++){
        orien += pow * extract_edge_orien(cube->edges[which_edge_at_pos(i, cube)], FB);
        pow *= 2;
    } 
    return orien;
}

/* 0, ..., 3**7 - 1 */
int
cube_to_coud_index(cube_t* cube){
    int orien = 0;
    int pow = 1;
    for (int i = 0; i < NCORNERS - 1; i++){
        orien += pow * extract_corner_orien(cube->corners[which_corner_at_pos(i, cube)], UD);
        pow *= 3;
    } 
    return orien;
}

/* 0, ..., 2217093120 - 1 = c(12, 4) * 2**11 * 3**7 - 1*/
unsigned int
cube_to_L_index(cube_t* cube){
    return (
        (unsigned int)cube_to_ece_index(cube)
      + (unsigned int)cube_to_eofb_index(cube) * 495
      + (unsigned int)cube_to_coud_index(cube) * 495 * 2048
    );
}

/*

nxopt:

factor of the index     // subgroup
9930                    // permuting U and D layer corners with fixed orientation
* (1, 24, 70, 1680)     // ep: () <=> (e-edges/UD-edges, UD-edges, U-edges/D-edges/e-edges, U-edges/D-edges)
* (16, 256, 2048)       // eo: (2**4, 2**8, 2**11) <=> (e-edges, UD-edges, all edges)
* 495                   // e-slice: keep the e-slice edges in the e-slice

My L group:
solved CO
arbitrary CP
solved EO
arbitrary EP, except: keep e-slice in e-slice
*/

/* helper functions */

static int
which_corner_at_pos(int pos, cube_t* cube){
    for (int k = 0; k < NCORNERS; k++){
      if (extract_corner_perm(cube->corners[k]) == pos) return k;
    }
    return -1;
}

static int
which_edge_at_pos(int pos, cube_t* cube){
    for (int k = 0; k < NEDGES; k++){
      if (extract_edge_perm(cube->edges[k]) == pos) return k;
    }
    return -1;
}