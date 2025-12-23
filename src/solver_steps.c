#include "solver_steps.h"
#include "heuristic_functions.h"

solving_step fin = {
    .solving_type = SOLVE_FIN,
    .cube_is_solved = cube_state_is_solved,
    .p_data = &ptable_data_opt1,
    .heuristic_func = NULL      // use the optimized solver, it prunes on its own
};

solving_step htr = {
    .solving_type = SOLVE_HTR,
    .cube_is_solved = cube_state_is_htr,
    .p_data = &ptable_data_dr,
    .heuristic_func = &htr_heuristic
};

solving_step dr = {
    .solving_type = SOLVE_DR,
    .cube_is_solved = cube_state_is_dr,
    .p_data = &ptable_data_dr,
    .heuristic_func = &dr_heuristic
};

solving_step eo = {
    .solving_type = SOLVE_EO,
    .cube_is_solved = cube_state_is_eo,
    .p_data = NULL,
    .heuristic_func = NULL      // we do not prune when solving eo
};



// temp
// solving_step cross_D = {
//     .solving_type = SOLVE_CROSS_D,
//     .cube_is_solved = cube_state_is_cross_D,
//     .p_data = NULL,
//     .heuristic_func = NULL      // we do not prune when solving cross
// };
//
//
// solving_step xcross_D = {
//     .solving_type = SOLVE_XCROSS_D,
//     .cube_is_solved = cube_state_is_xcross_D,
//     .p_data = NULL,
//     .heuristic_func = NULL      // we do not prune when solving cross
// };
//
//
// solving_step xxcross_D = {
//     .solving_type = SOLVE_XXCROSS_D,
//     .cube_is_solved = cube_state_is_xxcross_D,
//     .p_data = NULL,
//     .heuristic_func = NULL      // we do not prune when solving cross
// };
//
//
// solving_step xxxcross_D = {
//     .solving_type = SOLVE_XXXCROSS_D,
//     .cube_is_solved = cube_state_is_xxxcross_D,
//     .p_data = NULL,
//     .heuristic_func = NULL      // we do not prune when solving cross
// };
//
//
// solving_step xxxxcross_D = {
//     .solving_type = SOLVE_XXXXCROSS_D,
//     .cube_is_solved = cube_state_is_xxxxcross_D,
//     .p_data = NULL,
//     .heuristic_func = NULL      // we do not prune when solving cross
// };
