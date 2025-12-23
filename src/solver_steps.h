#ifndef SOLVER_STEPS_H
#define SOLVER_STEPS_H

#include <stdio.h>
#include "core/cube.h"
#include "tables_ptable_data.h"
#include "core/cube_state.h"

enum solving_step_type {SOLVE_FIN, SOLVE_HTR, SOLVE_DR, SOLVE_EO, SOLVE_CROSS_D, SOLVE_XCROSS_D, SOLVE_XXCROSS_D, SOLVE_XXXCROSS_D, SOLVE_XXXXCROSS_D};

typedef struct solving_step {
    // char name[FILENAME_MAX];
    enum solving_step_type solving_type;
    bool (*cube_is_solved)(cube_t* c);
    ptable_data_t* p_data;
    int (*heuristic_func)(cube_t* c, ptable_data_t* p_data);
    /* do we want to restrict the moveset? */
    /* do we want to restrict the pruning table options? */
} solving_step;

extern  solving_step fin;
extern  solving_step htr;
extern  solving_step dr;
extern  solving_step eo;

extern  solving_step cross_D;
extern  solving_step xcross_D;
extern  solving_step xxcross_D;
extern  solving_step xxxcross_D;
extern  solving_step xxxxcross_D;

#endif /* SOLVER_STEPS_H */
