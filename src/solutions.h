#ifndef SOLUTIONS_H
#define SOLUTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* =========================
   Single Solution (dynamic)
   ========================= */

typedef struct {
    int* moves;
    int length;
    int capacity;
} Solution;

void solution_init(Solution* s);
void solution_free(Solution* s);
void solution_append(Solution* s, int move);
bool solution_pop(Solution* s);
Solution solution_copy(const Solution* src);

/* =========================
   Solution Set (many solutions)
   ========================= */

typedef struct {
    Solution* data;
    int count;
    int capacity;
} SolutionSet;

void solutionset_init(SolutionSet* set, int capacity);
void solutionset_free(SolutionSet* set);
int solutionset_add_copy(SolutionSet* set, const Solution* s);

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
void solutionset_merge_with_prefix(
    SolutionSet* dst,
    const SolutionSet* src,
    const Solution* prefix
);

void solutionset_trim_shortest(SolutionSet* set, int N);

#endif /* SOLUTIONS_H */
