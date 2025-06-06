#ifndef __CUBE_H_
#define __CUBE_H_

#include <stdbool.h>

#include "../../include/core/cube.h"

#define NSYMS 48
#define NMOVES 18
#define NMOVES_EXTENDED 55
#define NAXES 3
#define NEDGECUBIES 128    // edges: 3 bits for eo, 4 bits for ep -> 2**7
#define NCORNERCUBIES 512  // corners: 6 bits for co, 3 bits for cp -> 2**9
#define NFACES 6
#define NTWISTS 3

#define NCCU 70
#define NCO 2187
#define NECE 495
#define NEO 2048
#define NCCLASS 9930

enum corner {
  // Stickers with COUD=0, these are used for cp-indeces
  UBL,
  UBR,
  UFR,
  UFL,
  DFL,
  DFR,
  DBR,
  DBL,

  // Stickers with COUD=1 (rotated CW)
  LUB,
  BUR,
  RUF,
  FUL,
  LDF,
  FDR,
  RDB,
  BDL,

  // Stickers with COUD=2 (rotated CCW)
  BUL,
  RUB,
  FUR,
  LUF,
  FDL,
  RDF,
  BDR,
  LDB
};

enum edge {
  // Stickers with EOFB=0, these are used for ep-indeces
  UB,
  UR,
  UF,
  UL,
  DF,
  DR,
  DB,
  DL,
  BL,
  BR,
  FR,
  FL,

  // Stickers with EOFB=1 (misoriented edges)
  BU,
  RU,
  FU,
  LU,
  FD,
  RD,
  BD,
  LD,
  LB,
  RB,
  RF,
  LF,
};

typedef enum axes {
  FB,
  LR,
  UD,
} axes;

/* helper functions for manipulating and extracting info from the cubies. */

static inline int extract_corner_orien(uint16_t corner, axes a) {
  return 3 & (corner >> (3 + 2 * a));
}

static inline int extract_corner_perm(uint16_t corner) { return 7 & corner; }

static inline int extract_edge_orien(uint16_t edge, axes a) {
  return 1 & (edge >> (2 - a));
}

static inline int extract_edge_perm(uint16_t edge) { return 15 & (edge >> 3); }

static inline void update_edge_perm(uint16_t* edge, int ep) {
  *edge &= 7;
  *edge |= (uint16_t)((ep & 15) << 3);  // just to be safe
}

static inline void update_edge_orien(uint16_t* edge, int eo_fb, int eo_lr,
                                     int eo_ud) {
  *edge &= 120;  // 0b1111000
  *edge |= (uint16_t)(((eo_fb & 1) << 2) | ((eo_lr & 1) << 1) | (eo_ud & 1));
}

static inline void update_corner_perm(uint16_t* corner, int cp) {
  *corner &= 504;                 // 0b111111000
  *corner |= (uint16_t)(cp & 7);  // just to be safe
}

static inline void update_corner_orien(uint16_t* corner, int co_fb, int co_lr,
                                       int co_ud) {
  *corner &= 7;
  *corner |=
      (uint16_t)(((co_ud & 3) << 7) | ((co_lr & 3) << 5) | ((co_fb & 3) << 3));
}

/* helper functions for building cubies. */
static inline uint16_t build_corner(int cp, int co_fb, int co_lr, int co_ud) {
  uint16_t c = 0;
  update_corner_orien(&c, co_fb, co_lr, co_ud);
  update_corner_perm(&c, cp);
  return c;
}

static inline uint16_t build_edge(int ep, int eo_fb, int eo_lr, int eo_ud) {
  uint16_t e = 0;
  update_edge_orien(&e, eo_fb, eo_lr, eo_ud);
  update_edge_perm(&e, ep);
  return e;
}

/* Given a cube with valid permutations and valid eo and co along FB.
These functions will set the correct orientation bits for the other axes. */

void fix_co_lr_ud(cube_t* cube);

// BIGBIG TODO: test this!
void fix_co_fb_lr(cube_t* cube);

void fix_eo_lr_ud(cube_t* cube);

/* swaps two cubes */
void swap_cubes(cube_t* c, cube_t* t);

/* find the edge/corner that occupies a given position */
int which_corner_at_pos(int pos, cube_t* cube);
int which_edge_at_pos(int pos, cube_t* cube);

void build_pos_to_edge(const cube_t* cube, int pos_to_edge[NEDGES]);
void build_pos_to_corner(const cube_t* cube, int pos_to_corner[NCORNERS]);

#endif /* __CUBE_H_ */
