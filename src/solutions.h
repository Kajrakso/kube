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

#endif /* SOLUTIONS_H */
