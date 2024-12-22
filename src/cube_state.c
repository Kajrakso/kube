#include "_cube.h"

bool cube_state_is_solved(cube_t *cube)
{
  for (int i = 0; i < NEDGES - 1; i++)
  {
    if (extract_edge_perm(cube->edges[i]) != i)
    {
      return false;
    }
    if (extract_edge_orien(cube->edges[i], FB) != 0)
    {
      return false;
    }
  }

  for (int i = 0; i < NCORNERS - 1; i++)
  {
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

bool cube_state_is_dr(cube_t* cube){
  return cube_state_is_dr_FB(cube) || cube_state_is_dr_LR(cube) || cube_state_is_dr_UD(cube);
}

bool cube_state_is_htr(cube_t* cube){
    return true;    // todo: implement this
}
