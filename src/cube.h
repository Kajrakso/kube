#ifndef _CUBE_H_
#define _CUBE_H_

#include <stdint.h>
#include <stdbool.h>
#include "utilities.h"

#define NMOVES 18
#define NAXES 3
#define NEDGECUBIES 128  // edges: 3 bits for eo, 4 bits for ep -> 2**7
#define NCORNERCUBIES 512  // corners: 6 bits for co, 3 bits for cp -> 2**9
#define NCORNERS 8
#define NEDGES 12
#define NFACES 6
#define NTWISTS 3

typedef enum {
    UBL, UBR, UFR, UFL,
    DFL, DFR, DBR, DBL,
} corner;

typedef enum {
    UB, UR, UF, UL,
    DF, DR, DB, DL,
    BL, BR, FR, FL,
} edge;

typedef enum {
  FB, LR, UD,
} axes;

typedef struct {
  uint16_t edges[NEDGES];
  uint16_t corners[NCORNERS];
} cube_t;

cube_t cube_create_new_cube();

bool cube_valid_eo(cube_t* cube);
bool cube_valid_co(cube_t* cube);
bool cube_valid_ep(cube_t* cube);
bool cube_valid_cp(cube_t* cube);

int extract_corner_orien(uint16_t corner, axes a);
int extract_corner_perm(uint16_t corner);
int extract_edge_orien(uint16_t edge, axes a);
int extract_edge_perm(uint16_t edge);
void update_edge_perm(uint16_t* edge, int ep);
void update_edge_orien(uint16_t* edge, int o_fb, int o_lr, int o_ud);
void update_corner_perm(uint16_t* corner, int cp);
void update_corner_orien(uint16_t* corner, int o_fb, int o_lr, int o_ud);
uint16_t build_corner(int cp, int co_fb, int co_lr, int co_ud);
uint16_t build_edge(int ep, int eo_fb, int eo_lr, int eo_ud);

#endif /* _CUBE_H_ */