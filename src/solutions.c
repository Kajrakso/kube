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

bool solution_pop(Solution* s) {
    if (s->length == 0){
        return false;
    }
    else {
        s->length--;
        return true;
    }
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


void solutionset_append(SolutionSet* A, SolutionSet* B) {
    for (int i = 0; i < B->count; i++){
        solutionset_add_copy(A, &B->data[i]);
    }
}


void solutionset_merge_with_prefix(
    SolutionSet* dest,
    const SolutionSet* src,
    const Solution* prefix
){
    solutionset_init(dest, src->count);
    for (int i = 0; i < src->count; i++){
        solutionset_add_copy(dest, prefix);
    }

    for (int i = 0; i < src->count; i++) {
        Solution suffix = src->data[i];

        // append suffix
        for (int j = 0; j < suffix.length; j++) {
            solution_append(&dest->data[i], suffix.moves[j]);
        }

    }
}

static int solution_cmp_len(const void* a, const void* b) {
    const Solution* sa = a;
    const Solution* sb = b;
    return sa->length - sb->length;
}

void solutionset_trim_shortest(SolutionSet* set, int max_count){
    if (set->count <= max_count)
        return;

    // sort by increasing length
    qsort(set->data,
          set->count,
          sizeof(Solution),
          solution_cmp_len);

    // free the discarded solutions
    for (int i = max_count; i < set->count; i++) {
        solution_free(&set->data[i]);
    }

    set->count = max_count; 
}

/* =========================
   Example "solver"
   ========================= */

#include "cli.h"
void fake_solver(SolutionSet* out) {
    Solution s;
    solution_init(&s);
    solution_append(&s, 1);
    solution_append(&s, 2);
    solution_append(&s, 3);
    solutionset_add_copy(out, &s);
    solution_append(&s, 4);
    solutionset_add_copy(out, &s);
    solution_free(&s);
}

void fake_solver2(SolutionSet* out) {
    Solution s;
    solution_init(&s);
    solution_append(&s, 1);
    solution_append(&s, 2);
    solution_append(&s, 3);
    solution_append(&s, 6);
    solution_append(&s, 6);
    solutionset_add_copy(out, &s);
    solution_append(&s, 4);
    solutionset_add_copy(out, &s);
    solution_free(&s);
}



/* =========================
   Demo
   ========================= */

#include "cli.h"

int main_2() {
    SolutionSet solutions;
    solutionset_init(&solutions, 10);

    fake_solver2(&solutions);
    fake_solver(&solutions);

    
    printf("number of sols before: %i\n", solutions.count);
    cube_print_solution_set(&solutions, 0);
    solutionset_trim_shortest(&solutions, 3);
    printf("number of sols after: %i\n", solutions.count);
    cube_print_solution_set(&solutions, 0);

    solutionset_free(&solutions);
    return 0;
}

