#ifndef __CUBE_H_
#define __CUBE_H_

#include "../include/core/cube.h"
#include <stdbool.h>

#define NSYMS 48
#define NMOVES 18
#define NAXES 3
#define NEDGECUBIES 128  // edges: 3 bits for eo, 4 bits for ep -> 2**7
#define NCORNERCUBIES 512  // corners: 6 bits for co, 3 bits for cp -> 2**9
#define NFACES 6
#define NTWISTS 3
#define FACT8 40320

enum corner {
    UBL, UBR, UFR, UFL,
    DFL, DFR, DBR, DBL,
};

enum edge {
    UB, UR, UF, UL,
    DF, DR, DB, DL,
    BL, BR, FR, FL,
};

typedef enum axes {
  FB, LR, UD,
} axes;

/* helper functions for manipulating and extracting info from the cubies. */

int extract_corner_orien(uint16_t corner, axes a);
int extract_corner_perm(uint16_t corner);
int extract_edge_orien(uint16_t edge, axes a);
int extract_edge_perm(uint16_t edge);
void update_edge_perm(uint16_t* edge, int ep);
void update_edge_orien(uint16_t* edge, int eo_fb, int eo_lr, int eo_ud);
void update_corner_perm(uint16_t* corner, int cp);
void update_corner_orien(uint16_t* corner, int co_fb, int co_lr, int co_ud);

/* helper functions for building cubies. */

uint16_t build_corner(int cp, int co_fb, int co_lr, int co_ud);
uint16_t build_edge(int ep, int eo_fb, int eo_lr, int eo_ud);

/* Given a cube with valid permutations and valid eo and co along FB.
These functions will set the correct orientation bits for the other axes. */

void fix_co_lr_ud(cube_t* cube);
void fix_eo_lr_ud(cube_t* cube);

#endif /* __CUBE_H_ */