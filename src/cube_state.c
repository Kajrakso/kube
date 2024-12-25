#include "_cube_state.h"

/* private */

bool cube_valid_eo(cube_t* cube){
  int eo_sum_FB = 0, eo_sum_LR = 0, eo_sum_UD = 0;
  for (int i = 0; i < NEDGES; i++){
    eo_sum_FB += extract_edge_orien(cube->edges[i], FB);
    eo_sum_LR += extract_edge_orien(cube->edges[i], LR);
    eo_sum_UD += extract_edge_orien(cube->edges[i], UD);
  }
  return eo_sum_FB % 2 == 0 && eo_sum_LR % 2 == 0 && eo_sum_UD % 2 == 0;
}

bool cube_valid_co(cube_t* cube){
  int co_sum_FB = 0, co_sum_LR = 0, co_sum_UD = 0;
  for (int i = 0; i < NCORNERS; i++){
    co_sum_FB += extract_corner_orien(cube->corners[i], FB);
    co_sum_LR += extract_corner_orien(cube->corners[i], LR);
    co_sum_UD += extract_corner_orien(cube->corners[i], UD);
  }
  return co_sum_FB % 3 == 0 && co_sum_LR % 3 == 0 && co_sum_UD % 3 == 0;
}

// todo: implement these.
bool cube_valid_permutation(cube_t* cube){
  return true;
}


/* public */

bool cube_state_is_solved(cube_t *cube){
  return cube_state_is_corners_solved(cube) && cube_state_is_edges_solved(cube);
}

bool cube_state_is_corners_solved(cube_t* cube){
  for (int i = 0; i < NCORNERS - 1; i++){
    if (extract_corner_perm(cube->corners[i]) != i)
    {
      return false;
    }
    if (extract_corner_orien(cube->corners[i], FB) != 0)
    {
      return false;
    }
  }

  return true;
}

bool cube_state_is_edges_solved(cube_t* cube){
  for (int i = 0; i < NEDGES - 1; i++) {
    if (extract_edge_perm(cube->edges[i]) != i)
    {
      return false;
    }
    if (extract_edge_orien(cube->edges[i], FB) != 0)
    {
      return false;
    }
  }
}

bool cube_state_is_eo_FB(cube_t* cube){
  for (int i = 0; i < NEDGES - 1; i++)
  {
    if (extract_edge_orien(cube->edges[i], FB) != 0)
    {
      return false;
    }
  }
  return true;
}

bool cube_state_is_eo_LR(cube_t* cube){
  for (int i = 0; i < NEDGES - 1; i++)
  {
    if (extract_edge_orien(cube->edges[i], LR) != 0)
    {
      return false;
    }
  }
  return true;
}

bool cube_state_is_eo_UD(cube_t* cube){
  for (int i = 0; i < NEDGES - 1; i++)
  {
    if (extract_edge_orien(cube->edges[i], UD) != 0)
    {
      return false;
    }
  }
  return true;
}

bool cube_state_is_co_FB(cube_t* cube){
  for (int i = 0; i < NCORNERS - 1; i++)
  {
    if (extract_corner_orien(cube->corners[i], FB) != 0)
    {
      return false;
    }
  }
  return true;
}

bool cube_state_is_co_LR(cube_t* cube){
  for (int i = 0; i < NCORNERS - 1; i++)
  {
    if (extract_corner_orien(cube->corners[i], LR) != 0)
    {
      return false;
    }
  }
  return true;
}

bool cube_state_is_co_UD(cube_t* cube){
  for (int i = 0; i < NCORNERS - 1; i++)
  {
    if (extract_corner_orien(cube->corners[i], UD) != 0)
    {
      return false;
    }
  }
  return true;
}

bool cube_state_is_dr_FB(cube_t* cube){
  return cube_state_is_eo_LR(cube) && cube_state_is_eo_UD(cube) && cube_state_is_co_FB(cube);
}

bool cube_state_is_dr_LR(cube_t* cube){
  return cube_state_is_eo_FB(cube) && cube_state_is_eo_UD(cube) && cube_state_is_co_LR(cube);
}

bool cube_state_is_dr_UD(cube_t* cube){
  return cube_state_is_eo_FB(cube) && cube_state_is_eo_LR(cube) && cube_state_is_co_UD(cube);
}

bool cube_state_is_eo(cube_t* cube){
  return cube_state_is_eo_FB(cube) || cube_state_is_eo_LR(cube) || cube_state_is_eo_UD(cube);
}

bool cube_state_is_co(cube_t* cube){
  return cube_state_is_co_FB(cube) || cube_state_is_co_LR(cube) || cube_state_is_co_UD(cube);
}

bool cube_state_is_dr(cube_t* cube){
  return cube_state_is_dr_FB(cube) || cube_state_is_dr_LR(cube) || cube_state_is_dr_UD(cube);
}

bool cube_state_is_htr(cube_t* cube){
    return true;    // todo: implement this
}
