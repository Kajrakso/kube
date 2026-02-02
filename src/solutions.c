#include "solutions.h"

/* =========================
   Single Solution (dynamic)
   ========================= */

void solution_init(Solution* s) {
    s->length = 0;
    s->capacity = 32;  // initial capacity
    s->moves = malloc(sizeof(int) * s->capacity);
}

void solution_free(Solution* s) {
    free(s->moves);
    s->moves = NULL;
    s->length = 0;
    s->capacity = 0;
}

void solution_append(Solution* s, int move) {
    if (s->length >= s->capacity) {
        s->capacity *= 2;
        s->moves = realloc(s->moves, sizeof(int) * s->capacity);
    }
    s->moves[s->length++] = move;
}

Solution solution_copy(const Solution* src) {
    Solution dst;
    dst.length = src->length;
    dst.capacity = src->capacity;
    dst.moves = malloc(sizeof(int) * dst.capacity);
    memcpy(dst.moves, src->moves, sizeof(int) * src->length);
    return dst;
}

/* =========================
   Solution Set (many solutions)
   ========================= */

void solutionset_init(SolutionSet* set, int capacity) {
    set->count = 0;
    set->capacity = capacity;
    set->data = malloc(sizeof(Solution) * capacity);
}

void solutionset_free(SolutionSet* set) {
    for (int i = 0; i < set->count; i++) {
        solution_free(&set->data[i]);
    }
    free(set->data);
}

int solutionset_add_copy(SolutionSet* set, const Solution* s) {
    if (set->count >= set->capacity) {
        return 0; // full
    }
    set->data[set->count++] = solution_copy(s);
    return 1;
}

/* =========================
   Example "solver"
   ========================= */

void fake_solver(SolutionSet* out) {
    Solution s;
    solution_init(&s);

    solution_append(&s, 1);
    solution_append(&s, 2);
    solution_append(&s, 3);
    solutionset_add_copy(out, &s);

    solution_append(&s, 4);
    solution_append(&s, 5);


    solutionset_add_copy(out, &s);

    solution_free(&s);
}

/* =========================
   Demo
   ========================= */

#include "cli.h"

int main_demo() {
    SolutionSet solutions;
    solutionset_init(&solutions, 10);

    fake_solver(&solutions);


    for (int i = 0; i < solutions.count; i++) {
        Solution* s = &solutions.data[i];
        printf("Solution %d: ", i);

        for (int j = 0; j < s->length; j++) {
            printf("%d ", s->moves[j]);
        }
        printf("\n");
    }

    cube_print_solution_set(&solutions, 0);

    solutionset_free(&solutions);
    return 0;
}

