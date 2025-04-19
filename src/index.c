#include "_index.h"

/* 0, .., 495 - 1 = comb(12, 4) - 1 */
uint32_t
cube_to_ece_index(cube_t* cube){
    return combinatorials_lookup[
          extract_edge_perm(cube->edges[FR]) * 1
        + extract_edge_perm(cube->edges[FL]) * 12
        + extract_edge_perm(cube->edges[BL]) * 144
        + extract_edge_perm(cube->edges[BR]) * 1728
    ];
}

/* 0, ..., 2**11 - 1 */
uint32_t
cube_to_eofb_index(cube_t* cube){
    uint32_t orien = 0;
    int pow = 1;
    for (int i = 0; i < NEDGES - 1; i++){
        orien += pow * extract_edge_orien(cube->edges[which_edge_at_pos(i, cube)], FB);
        pow *= 2;
    } 
    return orien;
}

/* 0, ..., 3**7 - 1 */
uint32_t
cube_to_coud_index(cube_t* cube){
    uint32_t orien = 0;
    int pow = 1;
    for (int i = 0; i < NCORNERS - 1; i++){
        orien += pow * extract_corner_orien(cube->corners[which_corner_at_pos(i, cube)], UD);
        pow *= 3;
    } 
    return orien;
}

inline uint32_t
ece_eofb_coud_to_L_index(uint32_t ece, uint32_t eofb, uint32_t coud){
    return ece + eofb * NECE + coud * NECE * NEO;
}

/* 0, ..., 2217093120 - 1 = c(12, 4) * 2**11 * 3**7 - 1*/
uint32_t
cube_to_L_index(cube_t* cube){
    return ece_eofb_coud_to_L_index(
        cube_to_ece_index(cube),
        cube_to_eofb_index(cube),
        cube_to_coud_index(cube)
    );
}

/* helper functions */

int
which_corner_at_pos(int pos, cube_t* cube){
    for (int k = 0; k < NCORNERS; k++){
      if (extract_corner_perm(cube->corners[k]) == pos) return k;
    }
    return -1;
}

int
which_edge_at_pos(int pos, cube_t* cube){
    for (int k = 0; k < NEDGES; k++){
      if (extract_edge_perm(cube->edges[k]) == pos) return k;
    }
    return -1;
}
