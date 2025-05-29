#include "_cube.h"

/* public */

cube_t cube_create_new_cube() {
  cube_t cube = {
      .edges = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      .corners = {0, 0, 0, 0, 0, 0, 0, 0},
  };

  for (int i = 0; i < 12; i++) {
    update_edge_perm(&cube.edges[i], i);
    update_edge_orien(&cube.edges[i], 0, 0, 0);
  }

  for (int i = 0; i < 8; i++) {
    update_corner_perm(&cube.corners[i], i);
    update_corner_orien(&cube.corners[i], 0, 0, 0);
  }

  return cube;
}

/* private */

// returns the slice an edge belongs to.
int e_slice(int i);

enum slice { M, S, E };

void fix_co_lr_ud(cube_t* cube) {
  int i, perm, orien_fb;

  for (i = 0; i < NCORNERS; i++) {
    perm = extract_corner_perm(cube->corners[i]);
    orien_fb = extract_corner_orien(cube->corners[i], FB);

    if (perm % 2 == i % 2) {
      update_corner_orien(&cube->corners[i], orien_fb, orien_fb, orien_fb);
    } else {
      if (perm % 2 == 0) {
        update_corner_orien(&cube->corners[i], orien_fb, (2 + orien_fb) % 3,
                            (1 + orien_fb) % 3);
      } else {
        update_corner_orien(&cube->corners[i], orien_fb, (1 + orien_fb) % 3,
                            (2 + orien_fb) % 3);
      }
    }
  }
}

// have to test this
void fix_eo_lr_ud(cube_t* cube) {
  int i, perm, orien_fb;

  for (i = 0; i < NEDGES; i++) {
    perm = extract_edge_perm(cube->edges[i]);
    orien_fb = extract_edge_orien(cube->edges[i], FB);

    // ugly but it works. need to rewrite this.
    if (e_slice(i) == e_slice(perm)) {
      update_edge_orien(&cube->edges[i], orien_fb, orien_fb, orien_fb);
    } else if ((e_slice(i) == M && e_slice(perm) == E) ||
               (e_slice(i) == E && e_slice(perm) == M)) {
      update_edge_orien(&cube->edges[i], orien_fb, 1 ^ orien_fb, 1 ^ orien_fb);
    } else if ((e_slice(i) == M && e_slice(perm) == S) ||
               (e_slice(i) == S && e_slice(perm) == M)) {
      update_edge_orien(&cube->edges[i], orien_fb, orien_fb, 1 ^ orien_fb);
    } else if ((e_slice(i) == E && e_slice(perm) == S) ||
               (e_slice(i) == S && e_slice(perm) == E)) {
      update_edge_orien(&cube->edges[i], orien_fb, 1 ^ orien_fb, orien_fb);
    }
  }
}

int e_slice(int i) {
  if (i == UB || i == UF || i == DF || i == DB) {
    return M;
  } else if (i == UR || i == UL || i == DR || i == DL) {
    return S;
  } else if (i == FR || i == FL || i == BR || i == BL) {
    return E;
  } else {
    return -1;
  }
}

void swap_cubes(cube_t* c, cube_t* t) {
  cube_t aux = *t;
  *t = *c;
  *c = aux;
}

// TODO: remove these and use a precompouted pos_to_edge/pos_to_corner map
// instead
int which_corner_at_pos(int pos, cube_t* cube) {
  for (int k = 0; k < NCORNERS; k++) {
    if (extract_corner_perm(cube->corners[k]) == pos) return k;
  }
  return -1;
}

int which_edge_at_pos(int pos, cube_t* cube) {
  for (int k = 0; k < NEDGES; k++) {
    if (extract_edge_perm(cube->edges[k]) == pos) return k;
  }
  return -1;
}

// TODO: LIke these ones :)
void build_pos_to_edge(const cube_t* cube, int pos_to_edge[NEDGES]) {
  for (int k = 0; k < NEDGES; k++) {
    int pos = extract_edge_perm(cube->edges[k]);
    pos_to_edge[pos] = k;
  }
}

void build_pos_to_corner(const cube_t* cube, int pos_to_corner[NCORNERS]) {
  for (int k = 0; k < NCORNERS; k++) {
    int pos = extract_corner_perm(cube->corners[k]);
    pos_to_corner[pos] = k;
  }
}
