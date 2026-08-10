#ifndef SOLVER_STEPS_H
#define SOLVER_STEPS_H

#include <stdio.h>
#include "core/cube.h"
#include "tables_ptable_data.h"
#include "core/cube_state.h"

enum solving_step_type {
    SOLVE_FIN,
    SOLVE_HTR,
    SOLVE_DR,
    SOLVE_EO,
    SOLVE_CUSTOM,
    SOLVE_CROSS_D,
    SOLVE_XCROSS_D,
    SOLVE_XXCROSS_D,
    SOLVE_XXXCROSS_D,
    SOLVE_XXXXCROSS_D
};

typedef struct solving_step {
    char* name;
    enum solving_step_type solving_type;
    //bool (*cube_is_solved)(cube_t* c);
    bool (*cube_is_solved)(cube_t* c, void* data);
    ptable_data_t* p_data;
    size_t (*heuristic_func)(cube_t* c, ptable_data_t* p_data);
    uint32_t moveset_mask;

    /* do we want to restrict the pruning table options? */

    void* custom_data;
    bool is_custom;
    /* pruning tables for DSL (custom) steps: either one combined table or
     * one table per T1 component; the heuristic is max-composed over them */
    ptable_data_t** custom_ptables;
    uint8_t n_custom_ptables;

} solving_step;





extern solving_step fin;
extern solving_step htr;
extern solving_step dr;
extern solving_step eo;

extern solving_step cross_D;
extern solving_step xcross_D;
extern solving_step xxcross_D;
extern solving_step xxxcross_D;
extern solving_step xxxxcross_D;

#endif /* SOLVER_STEPS_H */
