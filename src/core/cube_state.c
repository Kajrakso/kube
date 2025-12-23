#include "cube_state.h"

/* private */

bool cube_valid_eo(cube_t* cube) {
    int eo_sum_FB = 0, eo_sum_LR = 0, eo_sum_UD = 0;
    for (int i = 0; i < NEDGES; i++)
    {
        eo_sum_FB += extract_edge_orien(cube->edges[i], FB);
        eo_sum_LR += extract_edge_orien(cube->edges[i], LR);
        eo_sum_UD += extract_edge_orien(cube->edges[i], UD);
    }
    return eo_sum_FB % 2 == 0 && eo_sum_LR % 2 == 0 && eo_sum_UD % 2 == 0;
}

bool cube_valid_co(cube_t* cube) {
    int co_sum_FB = 0, co_sum_LR = 0, co_sum_UD = 0;
    for (int i = 0; i < NCORNERS; i++)
    {
        co_sum_FB += extract_corner_orien(cube->corners[i], FB);
        co_sum_LR += extract_corner_orien(cube->corners[i], LR);
        co_sum_UD += extract_corner_orien(cube->corners[i], UD);
    }
    return co_sum_FB % 3 == 0 && co_sum_LR % 3 == 0 && co_sum_UD % 3 == 0;
}

// todo: implement these.
bool cube_valid_permutation(cube_t* cube) { return true; }

/* public */

bool cube_state_is_solved(cube_t* cube) {
    cube_t solved_cube = cube_create_new_cube();
    return cube_operation_is_equal(solved_cube, *cube);
}

bool cube_state_is_corners_solved(cube_t* cube) {
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

bool cube_state_is_edges_solved(cube_t* cube) {
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

    return true;
}

bool cube_state_is_eo_FB(cube_t* cube) {
    for (int i = 0; i < NEDGES - 1; i++)
    {
        if (extract_edge_orien(cube->edges[i], FB) != 0)
        {
            return false;
        }
    }
    return true;
}

bool cube_state_is_eo_LR(cube_t* cube) {
    for (int i = 0; i < NEDGES - 1; i++)
    {
        if (extract_edge_orien(cube->edges[i], LR) != 0)
        {
            return false;
        }
    }
    return true;
}

bool cube_state_is_eo_UD(cube_t* cube) {
    for (int i = 0; i < NEDGES - 1; i++)
    {
        if (extract_edge_orien(cube->edges[i], UD) != 0)
        {
            return false;
        }
    }
    return true;
}

bool cube_state_is_co_FB(cube_t* cube) {
    for (int i = 0; i < NCORNERS - 1; i++)
    {
        if (extract_corner_orien(cube->corners[i], FB) != 0)
        {
            return false;
        }
    }
    return true;
}

bool cube_state_is_co_LR(cube_t* cube) {
    for (int i = 0; i < NCORNERS - 1; i++)
    {
        if (extract_corner_orien(cube->corners[i], LR) != 0)
        {
            return false;
        }
    }
    return true;
}

bool cube_state_is_co_UD(cube_t* cube) {
    for (int i = 0; i < NCORNERS - 1; i++)
    {
        if (extract_corner_orien(cube->corners[i], UD) != 0)
        {
            return false;
        }
    }
    return true;
}

bool cube_state_is_dr_FB(cube_t* cube) {
    return cube_state_is_eo_LR(cube) && cube_state_is_eo_UD(cube) && cube_state_is_co_FB(cube);
}

bool cube_state_is_dr_LR(cube_t* cube) {
    return cube_state_is_eo_FB(cube) && cube_state_is_eo_UD(cube) && cube_state_is_co_LR(cube);
}

bool cube_state_is_dr_UD(cube_t* cube) {
    return cube_state_is_eo_FB(cube) && cube_state_is_eo_LR(cube) && cube_state_is_co_UD(cube);
}

bool cube_state_is_eo(cube_t* cube) {
    return cube_state_is_eo_FB(cube) || cube_state_is_eo_LR(cube) || cube_state_is_eo_UD(cube);
}

bool cube_state_is_ep(cube_t* cube) {
    for (int i = 0; i < NEDGES - 1; i++)
    {
        if (extract_edge_perm(cube->edges[i]) != i)
        {
            return false;
        }
    }
    return true;
}

bool cube_state_is_co(cube_t* cube) {
    return cube_state_is_co_FB(cube) || cube_state_is_co_LR(cube) || cube_state_is_co_UD(cube);
}

bool cube_state_is_cp(cube_t* cube) {
    for (int i = 0; i < NCORNERS - 1; i++)
    {
        if (extract_corner_perm(cube->corners[i]) != i)
        {
            return false;
        }
    }
    return true;
}


bool cube_state_is_dr(cube_t* cube) {
    return (cube_state_is_dr_FB(cube) || cube_state_is_dr_LR(cube) || cube_state_is_dr_UD(cube));
}


// bool cube_state_is_cross_D(cube_t* cube){
//     return (extract_edge_perm(cube->edges[DB]) == DB
//         && extract_edge_perm(cube->edges[DR]) == DR
//         && extract_edge_perm(cube->edges[DL]) == DL
//         && extract_edge_perm(cube->edges[DF]) == DF
//         && extract_edge_orien(cube->edges[DB], UD) == 0
//         && extract_edge_orien(cube->edges[DR], UD) == 0
//         && extract_edge_orien(cube->edges[DL], UD) == 0
//         && extract_edge_orien(cube->edges[DF], UD) == 0
//     );
// }
//
//
// bool cube_state_is_xcross_D(cube_t* cube){
//     cube_t c = *cube;
//     int num_pairs = 0;
//     for (int i = 0; i < 4; i++){
//         if (extract_edge_perm(c.edges[DB]) == DB
//             && extract_edge_perm(c.edges[DR]) == DR
//             && extract_edge_perm(c.edges[DL]) == DL
//             && extract_edge_perm(c.edges[DF]) == DF
//             && extract_edge_orien(c.edges[DB], UD) == 0
//             && extract_edge_orien(c.edges[DR], UD) == 0
//             && extract_edge_orien(c.edges[DL], UD) == 0
//             && extract_edge_orien(c.edges[DF], UD) == 0
//
//             && extract_edge_perm(c.edges[FL]) == FL
//             && extract_edge_orien(c.edges[FL], UD) == 0
//             && extract_corner_perm(c.corners[DFL] == DFL)
//             && extract_corner_orien(c.corners[DFL], UD) == 0
//         ) {
//             num_pairs++;
//         }
//
//         if (num_pairs == 1){
//             return true;
//         }
//
//         c = cube_operation_sym_conjugate(c, 1);
//     }
//     return false;
// }
//
//
// bool cube_state_is_xxcross_D(cube_t* cube){
//     cube_t c = *cube;
//     int num_pairs = 0;
//     for (int i = 0; i < 4; i++){
//         if (extract_edge_perm(c.edges[DB]) == DB
//             && extract_edge_perm(c.edges[DR]) == DR
//             && extract_edge_perm(c.edges[DL]) == DL
//             && extract_edge_perm(c.edges[DF]) == DF
//             && extract_edge_orien(c.edges[DB], UD) == 0
//             && extract_edge_orien(c.edges[DR], UD) == 0
//             && extract_edge_orien(c.edges[DL], UD) == 0
//             && extract_edge_orien(c.edges[DF], UD) == 0
//
//             && extract_edge_perm(c.edges[FL]) == FL
//             && extract_edge_orien(c.edges[FL], UD) == 0
//             && extract_corner_perm(c.corners[DFL] == DFL)
//             && extract_corner_orien(c.corners[DFL], UD) == 0
//         ) {
//             num_pairs++;
//         }
//
//         if (num_pairs == 2){
//             return true;
//         }
//
//         c = cube_operation_sym_conjugate(c, 1);
//     }
//     return false;
// }
//
// bool cube_state_is_xxxcross_D(cube_t* cube){
//     cube_t c = *cube;
//     int num_pairs = 0;
//     for (int i = 0; i < 4; i++){
//         if (extract_edge_perm(c.edges[DB]) == DB
//             && extract_edge_perm(c.edges[DR]) == DR
//             && extract_edge_perm(c.edges[DL]) == DL
//             && extract_edge_perm(c.edges[DF]) == DF
//             && extract_edge_orien(c.edges[DB], UD) == 0
//             && extract_edge_orien(c.edges[DR], UD) == 0
//             && extract_edge_orien(c.edges[DL], UD) == 0
//             && extract_edge_orien(c.edges[DF], UD) == 0
//
//             && extract_edge_perm(c.edges[FL]) == FL
//             && extract_edge_orien(c.edges[FL], UD) == 0
//             && extract_corner_perm(c.corners[DFL] == DFL)
//             && extract_corner_orien(c.corners[DFL], UD) == 0
//         ) {
//             num_pairs++;
//         }
//
//         if (num_pairs == 3){
//             return true;
//         }
//
//         c = cube_operation_sym_conjugate(c, 1);
//     }
//     return false;
// }
//
//
// bool cube_state_is_xxxxcross_D(cube_t* cube){
//     cube_t c = *cube;
//     int num_pairs = 0;
//     for (int i = 0; i < 4; i++){
//         if (extract_edge_perm(c.edges[DB]) == DB
//             && extract_edge_perm(c.edges[DR]) == DR
//             && extract_edge_perm(c.edges[DL]) == DL
//             && extract_edge_perm(c.edges[DF]) == DF
//             && extract_edge_orien(c.edges[DB], UD) == 0
//             && extract_edge_orien(c.edges[DR], UD) == 0
//             && extract_edge_orien(c.edges[DL], UD) == 0
//             && extract_edge_orien(c.edges[DF], UD) == 0
//
//             && extract_edge_perm(c.edges[FL]) == FL
//             && extract_edge_orien(c.edges[FL], UD) == 0
//             && extract_corner_perm(c.corners[DFL] == DFL)
//             && extract_corner_orien(c.corners[DFL], UD) == 0
//         ) {
//             num_pairs++;
//         }
//
//         if (num_pairs == 4){
//             return true;
//         }
//
//         c = cube_operation_sym_conjugate(c, 1);
//     }
//     return false;
// }





#include "../utils.h"
#include "../tables.h"

uint64_t cube_to_cp_index(cube_t* cube){
    int cp[NCORNERS];
    for (int i = 0; i < NCORNERS; i++){
        cp[i] = (int)extract_corner_perm(cube->corners[i]);
    }
    
    return perm_to_fact(cp, NCORNERS);
}


bool cube_state_is_htr(cube_t* cube) {
    int* dr_subsets = get_dr_subsets();

    uint64_t cp_i = cube_to_cp_index(cube);

    return cube_state_is_dr_FB(cube) && cube_state_is_dr_UD(cube) && cube_state_is_dr_LR(cube) && dr_subsets[cp_i] == 1;
}







