#include "_cube.h"

cube_t cube_create_new_cube(){
  cube_t cube = {
    .edges = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    .corners = {0, 0, 0, 0, 0, 0, 0, 0},
  };

  for (int i = 0; i < 12; i++){
    update_edge_perm(&cube.edges[i], i);
    update_edge_orien(&cube.edges[i], 0, 0, 0);
  }

  for (int i = 0; i < 8; i++){
    update_corner_perm(&cube.corners[i], i);
    update_corner_orien(&cube.corners[i], 0, 0, 0);
  }

  return cube;
}

int extract_corner_orien(uint16_t corner, axes a){
  return 3 & (corner >> (3 + 2 * a));
}

int extract_corner_perm(uint16_t corner){
  return 7 & corner;
}

int extract_edge_orien(uint16_t edge, axes a){
  return 1 & (edge >> (2 - a));
}

int extract_edge_perm(uint16_t edge){
  return 15 & (edge >> 3);
}

void update_edge_perm(uint16_t* edge, int ep){
  *edge &= 7;
  *edge |= (ep & 15) << 3;  // just to be safe
}

void update_edge_orien(uint16_t* edge, int o_fb, int o_lr, int o_ud){
  *edge &= 120;   // 0b1111000
  *edge |= (o_fb << 2) | (o_lr << 1) | o_ud;
}

void update_corner_perm(uint16_t* corner, int cp){
  *corner &= 504;  // 0b111111000
  *corner |= (cp & 7);  // just to be safe
}

void update_corner_orien(uint16_t* corner, int o_fb, int o_lr, int o_ud){
  *corner &= 7;
  *corner |= (o_ud << 7) | (o_lr << 5) | (o_fb << 3);
}

uint16_t build_corner(int cp, int co_fb, int co_lr, int co_ud){
  uint16_t c = 0;
  update_corner_orien(&c, co_fb, co_lr, co_ud);
  update_corner_perm(&c, cp);
  return c;
}

uint16_t build_edge(int ep, int eo_fb, int eo_lr, int eo_ud){
  uint16_t e = 0;
  update_edge_orien(&e, eo_fb, eo_lr, eo_ud);
  update_edge_perm(&e, ep);
  return e;
}