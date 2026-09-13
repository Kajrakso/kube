#include "solver_steps.h"
#include "heuristic_functions.h"
#include "moveset.h"


static bool is_solved_wrap(cube_t* c, void* data) { (void)data; return cube_state_is_solved(c); }
static bool is_htr_wrap    (cube_t* c, void* data) { (void)data; return cube_state_is_htr(c); }
static bool is_dr_wrap     (cube_t* c, void* data) { (void)data; return cube_state_is_dr(c); }
static bool is_eo_wrap     (cube_t* c, void* data) { (void)data; return cube_state_is_eo(c); }


solving_step fin = {
    .name               = "fin",
    .solving_type       = SOLVE_FIN,
    .cube_is_solved     = is_solved_wrap,
    .p_data             = &ptable_data_opt1,
    .heuristic_func     = NULL,  // use the optimized solver, it prunes on its own
    .moveset_mask       = MOVESET_HTM
};
solving_step htr = {
    .name               = "htr",
    .solving_type       = SOLVE_HTR,
    .cube_is_solved     = is_htr_wrap,
    .p_data             = &ptable_data_dr,
    .heuristic_func     = &htr_heuristic,
    .moveset_mask       = MOVESET_HTM
};
solving_step dr  = {
    .name               = "dr",
    .solving_type       = SOLVE_DR,
    .cube_is_solved     = is_dr_wrap,
    .p_data             = &ptable_data_dr,
    .heuristic_func     = &dr_heuristic,
    .moveset_mask       = MOVESET_HTM
};
solving_step eo  = {
    .name               = "eo",
    .solving_type       = SOLVE_EO,
    .cube_is_solved     = is_eo_wrap,
    .p_data             = NULL,
    .heuristic_func     = NULL,
    .moveset_mask       = MOVESET_HTM
};

