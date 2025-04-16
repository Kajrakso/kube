#include "_index.h"

static int which_corner_at_pos(int pos, cube_t* cube){
    for (int k = 0; k < NCORNERS; k++){
      if (extract_corner_perm(cube->corners[k]) == pos) return k;
    }
    return -1;
}

static int which_edge_at_pos(int pos, cube_t* cube){
    for (int k = 0; k < NEDGES; k++){
      if (extract_edge_perm(cube->edges[k]) == pos) return k;
    }
    return -1;
}

/*
int
cube_to_cp_index(cube_t* cube){
    int perm[NCORNERS];
    for (int i = 0; i < NCORNERS; i++){
        perm[i] = extract_corner_perm(cube->corners[i]);
    } 
    return perm_to_fact(perm, NCORNERS);
}
*/

/*
int
cube_to_cofb_index(cube_t* cube){
    int orien = 0;
    int pow = 1;
    for (int i = 0; i < NCORNERS - 1; i++){
        orien += pow * extract_corner_orien(cube->corners[which_corner_at_pos(i, cube)], FB);
        pow *= 3;
    } 
    return orien;
}
*/

/*
int
cube_to_colr_index(cube_t* cube){
    int orien = 0;
    int pow = 1;
    for (int i = 0; i < NCORNERS - 1; i++){
        orien += pow * extract_corner_orien(cube->corners[which_corner_at_pos(i, cube)], LR);
        pow *= 3;
    } 
    return orien;
}
*/

/* 0, ..., 3**7 - 1*/
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


/* edge combination e slice: size: comb(12, 4): 0, .., 494 */
// int
// cube_to_ece_index(cube_t* cube){
//     int c = 1;
//     int res = 0;
//     for (int e = 0; e < NEDGES; e++){
//         int ep = which_edge_at_pos(e, cube);
//         if (ep == FR || ep == FL || ep == BL || ep == BR){
//             res += comb(e, c);
//             c++;
//         }
//     }

//     return res;
// }

/* edge combination e slice: size: comb(12, 4): 0, .., 494 */
int
cube_to_ece_index(cube_t* cube){
    return combinatorials_lookup[
          extract_edge_perm(cube->edges[FR]) * 1
        + extract_edge_perm(cube->edges[FL]) * 12
        + extract_edge_perm(cube->edges[BL]) * 144
        + extract_edge_perm(cube->edges[BR]) * 1728
    ];
}

/* edge combination U layer: size: comb(12, 4): 0, .., 494 */
/*
int
cube_to_ecU_index(cube_t* cube){
    int c = 1;
    int res = 0;
    for (int e = 0; e < NEDGES; e++){
        int ep = which_edge_at_pos(e, cube);
        if (ep == UB || ep == UR || ep == UF || ep == UL){
            res += comb(e, c);
            c++;
        }
    }

    return res;
}
*/

/* edge combination D layer: size: comb(12, 4): 0, .., 494 */
/*
int
cube_to_ecD_index(cube_t* cube){
    int c = 1;
    int res = 0;
    for (int e = 0; e < NEDGES; e++){
        int ep = which_edge_at_pos(e, cube);
        if (ep == DF || ep == DR || ep == DB || ep == DL){
            res += comb(e, c);
            c++;
        }
    }

    return res;
}
*/

/* edge orientation e slice: size: 2**4: 0, .., 15.
is this correct? */
// int
// cube_to_eofb_e_index(cube_t* cube){
//     return extract_edge_orien(cube->edges[BL], FB) +
//         extract_edge_orien(cube->edges[BR], FB) * 2 +
//         extract_edge_orien(cube->edges[FR], FB) * 4 +
//         extract_edge_orien(cube->edges[FL], FB) * 8;
// }

/*
int
cube_to_eofb_e_index(cube_t* cube){
    int orien = 0;
    int pow = 1;
    for (int i = 0; i < NEDGES - 1; i++){
        int edge = which_edge_at_pos(i, cube);
        if (edge == BL || edge == BR || edge == FR || edge == FL) {
            orien += pow * extract_edge_orien(cube->edges[edge], FB);
            pow *= 2;
        }
    } 
    return orien;
}
*/

/* edge orientation UD layers: 2**8: 0, ..., 255
is this correct? */
// int
// cube_to_eofb_UD_index(cube_t* cube){
//     return extract_edge_orien(cube->edges[UB], FB) +
//         extract_edge_orien(cube->edges[UR], FB) * 2 +
//         extract_edge_orien(cube->edges[UF], FB) * 4 +
//         extract_edge_orien(cube->edges[UL], FB) * 8 +
//         extract_edge_orien(cube->edges[DF], FB) * 16 +
//         extract_edge_orien(cube->edges[DR], FB) * 32 +
//         extract_edge_orien(cube->edges[DB], FB) * 64 +
//         extract_edge_orien(cube->edges[DL], FB) * 128 ;
// }

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
/*
int
cube_to_eofb_UD_index(cube_t* cube){
    int orien = 0;
    int pow = 1;
    for (int i = 0; i < NEDGES; i++){
        int edge = which_edge_at_pos(i, cube);
        if (edge == BL || edge == BR || edge == FR || edge == FL) continue;
        else {
            orien += pow * extract_edge_orien(cube->edges[edge], FB);
            pow *= 2;
        }
    } 
    return orien;
}
*/

/* 0, ..., 17321040 - 1 */
/*
unsigned int cube_to_H_index(cube_t* cube){
    return cube_to_ece_index(cube)
        + cube_to_eofb_e_index(cube) * 495
        + cube_to_coud_index(cube) * 495 * 16;

    // 495*16*3**7
}
*/

/* 0, ..., 277136640 - 1 */
/*
unsigned int cube_to_K_index(cube_t* cube){
    return cube_to_ece_index(cube)
        + cube_to_eofb_UD_index(cube) * 495
        + cube_to_coud_index(cube) * 495 * 256;

    // 495*2**8*3**7
}
*/

/*
2217093120
794299104
1855191742
398577124
*/

/* 0, ..., 2217093120 - 1 */
unsigned int cube_to_L_index(cube_t* cube){
    return (unsigned int)cube_to_ece_index(cube)
      + (unsigned int)cube_to_eofb_index(cube) * 495
      + (unsigned int)cube_to_coud_index(cube) * 495 * 2048;
    // 495*2048*3**7
}
/*

nxopt:

factor of the index     // subgroup
9930                    // permuting U and D layer corners with fixed orientation
* (1, 24, 70, 1680)     // ep: () <=> (e-edges/UD-edges, UD-edges, U-edges/D-edges/e-edges, U-edges/D-edges)
* (16, 256, 2048)       // eo: (2**4, 2**8, 2**11) <=> (e-edges, UD-edges, all edges)
* 495                   // e-slice: keep the e-slice edges in the e-slice


My H group:
fix corner orientation
permute corners arbitrarily
fix eo of e-slice edges
permute edges arbitrarliy, but keep e-slice in e-slice

My L group:
fix corner orientation
permute corners arbitrarily
solved eo
permute edges arbitrarliy, but keep e-slice in e-slice


*/
