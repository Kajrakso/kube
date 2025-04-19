#include "_cube.h"

/* public */

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

/* private */

// returns the slice an edge belongs to.
int e_slice(int i);

enum slice {
  M, S, E
};

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
  *edge |= (ep & 15) << 3; // just to be safe
}

void update_edge_orien(uint16_t* edge, int eo_fb, int eo_lr, int eo_ud){
  *edge &= 120; // 0b1111000
  *edge |= (eo_fb << 2) | (eo_lr << 1) | eo_ud;
}

void update_corner_perm(uint16_t* corner, int cp){
  *corner &= 504;      // 0b111111000
  *corner |= (cp & 7); // just to be safe
}

void update_corner_orien(uint16_t* corner, int co_fb, int co_lr, int co_ud){
  *corner &= 7;
  *corner |= (co_ud << 7) | (co_lr << 5) | (co_fb << 3);
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

void fix_co_lr_ud(cube_t* cube){
  int i, perm, orien_fb;

  for (i = 0; i < NCORNERS; i++){
    perm = extract_corner_perm(cube->corners[i]);
    orien_fb = extract_corner_orien(cube->corners[i], FB);

    if (perm % 2 == i % 2){
      update_corner_orien(&cube->corners[i], orien_fb, orien_fb, orien_fb);
    }
    else {
      if (perm % 2 == 0){
        update_corner_orien(&cube->corners[i], orien_fb, (2 + orien_fb) % 3, (1 + orien_fb) % 3);
      } else {
        update_corner_orien(&cube->corners[i], orien_fb, (1 + orien_fb) % 3, (2 + orien_fb) % 3);
      }
    }
  }
}

// have to test this
void fix_eo_lr_ud(cube_t* cube){
  int i, perm, orien_fb;

  for (i = 0; i < NEDGES; i++){
    perm = extract_edge_perm(cube->edges[i]);
    orien_fb = extract_edge_orien(cube->edges[i], FB);

    // ugly but it works. need to rewrite this.
    if (e_slice(i) == e_slice(perm)) {
      update_edge_orien(&cube->edges[i], orien_fb, orien_fb, orien_fb);
    } else if ((e_slice(i) == M && e_slice(perm) == E)
            || (e_slice(i) == E && e_slice(perm) == M)){
      update_edge_orien(&cube->edges[i], orien_fb, 1 ^ orien_fb, 1 ^ orien_fb);
    } else if ((e_slice(i) == M && e_slice(perm) == S)
            || (e_slice(i) == S && e_slice(perm) == M)){
      update_edge_orien(&cube->edges[i], orien_fb, orien_fb, 1 ^ orien_fb);
    } else if ((e_slice(i) == E && e_slice(perm) == S)
            || (e_slice(i) == S && e_slice(perm) == E)){
      update_edge_orien(&cube->edges[i], orien_fb, 1 ^ orien_fb, orien_fb);
    }
  }
}

int e_slice(int i){
  if (i == UB || i == UF || i == DF || i == DB){
    return M;
  }
  else if (i == UR || i == UL || i == DR || i == DL){
    return S;
  }
  else if (i == FR || i == FL || i == BR || i == BL){
    return E;
  }
  else {
    return -1;
  }
}

void
swap_cubes(cube_t* c, cube_t* t){
 cube_t aux = *t;
 *t = *c;
 *c = aux;
}
