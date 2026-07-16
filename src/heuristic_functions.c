#include "heuristic_functions.h"

size_t dr_heuristic(cube_t* cube, ptable_data_t* p_data){
    uint64_t      p1      = p_data->cube_to_index_func(cube, UD);
    size_t pval_UD = p_data->read_value_ptable_func(p1, p_data->ptable);
    uint64_t      p2      = p_data->cube_to_index_func(cube, LR);
    size_t  pval_LR = p_data->read_value_ptable_func(p2, p_data->ptable);
    uint64_t      p3      = p_data->cube_to_index_func(cube, FB);
    size_t  pval_FB = p_data->read_value_ptable_func(p3, p_data->ptable);
    
    size_t min = pval_UD;
    if (pval_FB < min){
        min = pval_FB;
    }
    if (pval_LR < min){
        min = pval_LR;
    }

    return min;
}


size_t htr_heuristic(cube_t* cube, ptable_data_t* p_data){
    uint64_t      p1      = p_data->cube_to_index_func(cube, UD);
    size_t pval_UD = p_data->read_value_ptable_func(p1, p_data->ptable);
    uint64_t      p2      = p_data->cube_to_index_func(cube, LR);
    size_t  pval_LR = p_data->read_value_ptable_func(p2, p_data->ptable);
    uint64_t      p3      = p_data->cube_to_index_func(cube, FB);
    size_t  pval_FB = p_data->read_value_ptable_func(p3, p_data->ptable);
    
    size_t max = pval_UD;
    if (pval_FB > max){
        max = pval_FB;
    }
    if (pval_LR > max){
        max = pval_LR;
    }

    return max;
}
