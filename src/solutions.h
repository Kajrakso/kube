#ifndef SOLUTIONS_H
#define SOLUTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "core/cube.h"
#include "solver_steps.h"

/* =========================
   Single Solution (dynamic)
   ========================= */

typedef struct {
    int* moves;
    int  length;
    int  capacity;
} Solution;

void     solution_init(Solution* s);
void     solution_free(Solution* s);
void     solution_append(Solution* s, int move);
bool     solution_pop(Solution* s);
Solution solution_copy(const Solution* src);

/* =========================
   Solution Set (many solutions)
   ========================= */

typedef struct {
    Solution* data;
    int       count;
    int       capacity;
} SolutionSet;

void solutionset_init(SolutionSet* set, int capacity);
void solutionset_free(SolutionSet* set);
int  solutionset_add_copy(SolutionSet* set, const Solution* s);

/* append solutionset B onto A. */
void solutionset_append(SolutionSet* A, SolutionSet* B);

/* You have:
a partial solution (the prefix)

P = [R U R']

a set of solutions for the next step, found by the solver

S = {
    [F],
    [F R],
    [F U']
}

What you want to generate is:

P + S = {
    [R U R' F],
    [R U R' F R],
    [R U R' F U']
}
 * */
void solutionset_merge_with_prefix(SolutionSet*       dst,
                                   const SolutionSet* src,
                                   const Solution*    prefix);


/* =========================
   Structures for use in pipeline solver
   ========================= */


/* =========================
   PipelineSolutionStep
   ========================= */

typedef struct {
    Solution* solution;
    bool      starts_on_inverse;
} PipelineSolutionStep;

void                 pipelinesolutionstep_init(PipelineSolutionStep* s, bool starts_on_inverse);
void                 pipelinesolutionstep_free(PipelineSolutionStep* s);
void                 pipelinesolutionstep_append(PipelineSolutionStep* s, int move);
bool                 pipelinesolutionstep_pop(PipelineSolutionStep* s);
PipelineSolutionStep pipelinesolutionstep_copy(const PipelineSolutionStep* src);


/* =========================
   PipelineSolution (dynamic)
   ========================= */

typedef struct {
    PipelineSolutionStep* steps;
    int                   count;
    int                   capacity;

    // we want to rank PipelineSolutions based on some heuristic (not just the length)
    int heuristic_score; 
} PipelineSolution;

void             pipelinesolution_init(PipelineSolution* pipeline_solution);
void             pipelinesolution_free(PipelineSolution* pipeline_solution);
int              pipelinesolution_add_copy(PipelineSolution*           set,
                                           const PipelineSolutionStep* pipeline_solution_step);
PipelineSolution pipelinesolution_copy(const PipelineSolution* src);

/* Apply the pipeline solution to the cube */
void apply_pipelinesolution(cube_t* cube, PipelineSolution* ps);

/* =========================
   PipelineSolutionSet
   ========================= */

typedef struct {
    PipelineSolution* data;
    int               count;
    int               capacity;
} PipelineSolutionSet;

void pipelinesolutionset_init(PipelineSolutionSet* set, int capacity);
void pipelinesolutionset_free(PipelineSolutionSet* set);
int  pipelinesolutionset_add_copy(PipelineSolutionSet* set, const PipelineSolution* s);

/* Add pipeline solution set B to A */
void pipelinesolutionset_append(PipelineSolutionSet* A, PipelineSolutionSet* B);

/* You have:
a partial solution (the prefix)

P = [R U R']

a set of solutions for the next step, found by the solver

S = {
    [F],
    [F R],
    [F U']
}

What you want to generate is:

P + S = {
    [R U R' F],
    [R U R' F R],
    [R U R' F U']
}
 * */
void pipelinesolutionset_merge_with_prefix(PipelineSolutionSet*       dst,
                                           const PipelineSolutionSet* src,
                                           const PipelineSolution*    prefix);


void pipelinesolutionset_compute_scores(cube_t c, PipelineSolutionSet* next, solving_step* next_ss);

void pipelinesolutionset_trim_shortest(PipelineSolutionSet* set, int max_count);

/* adds a solution set to a pipeline solution set,
 * such that each pipeline solution only has one step.
 * indicate if it is on normal or inverse via argument. */
void append_copy_solutionset_to_pipelinesolutionset(PipelineSolutionSet* pss, SolutionSet* ss, bool starts_on_inverse);

#endif /* SOLUTIONS_H */
