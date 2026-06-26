#include "solutions.h"

/* =========================
   Single Solution (dynamic)
   ========================= */

void solution_init(Solution* s) {
    s->length   = 0;
    s->capacity = 32;  // initial capacity
    s->moves    = malloc(sizeof(int) * s->capacity);
}

void solution_free(Solution* s) {
    free(s->moves);
    s->moves    = NULL;
    s->length   = 0;
    s->capacity = 0;
}

void solution_append(Solution* s, int move) {
    if (s->length >= s->capacity)
    {
        s->capacity *= 2;
        s->moves = realloc(s->moves, sizeof(int) * s->capacity);
    }
    s->moves[s->length++] = move;
}


void solution_append_multiple(Solution* s, int* moves, int number_of_moves){
    for (int i = 0; i < number_of_moves; i++){
        solution_append(s, moves[i]);
    }
}

bool solution_pop(Solution* s) {
    if (s->length == 0)
    {
        return false;
    }
    else
    {
        s->length--;
        return true;
    }
}

Solution solution_copy(const Solution* src) {
    Solution dst;
    dst.length   = src->length;
    dst.capacity = src->capacity;
    dst.moves    = malloc(sizeof(int) * dst.capacity);
    memcpy(dst.moves, src->moves, sizeof(int) * src->length);
    return dst;
}

/* =========================
   Solution Set (many solutions)
   ========================= */

void solutionset_init(SolutionSet* set, int capacity) {
    set->count    = 0;
    set->capacity = capacity;
    set->data     = malloc(sizeof(Solution) * capacity);
}

void solutionset_free(SolutionSet* set) {
    for (int i = 0; i < set->count; i++)
    {
        solution_free(&set->data[i]);
    }
    free(set->data);
}

int solutionset_add_copy(SolutionSet* set, const Solution* s) {
    if (set->count >= set->capacity)
    {
        return 0;  // full
    }
    set->data[set->count++] = solution_copy(s);
    return 1;
}


void solutionset_append(SolutionSet* A, SolutionSet* B) {
    for (int i = 0; i < B->count; i++)
    {
        solutionset_add_copy(A, &B->data[i]);
    }
}


void solutionset_merge_with_prefix(SolutionSet*       dest,
                                   const SolutionSet* src,
                                   const Solution*    prefix) {
    solutionset_init(dest, src->count);
    for (int i = 0; i < src->count; i++)
    {
        solutionset_add_copy(dest, prefix);
    }

    for (int i = 0; i < src->count; i++)
    {
        Solution suffix = src->data[i];

        // append suffix
        for (int j = 0; j < suffix.length; j++)
        {
            solution_append(&dest->data[i], suffix.moves[j]);
        }
    }
}

static int solution_cmp_len(const void* a, const void* b) {
    const Solution* sa = a;
    const Solution* sb = b;
    return sa->length - sb->length;
}

void solutionset_trim_shortest(SolutionSet* set, int max_count) {
    if (set->count <= max_count)
        return;

    // sort by increasing score
    qsort(set->data, set->count, sizeof(Solution), solution_cmp_len);

    // free the discarded solutions
    for (int i = max_count; i < set->count; i++)
    {
        solution_free(&set->data[i]);
    }

    set->count = max_count;
}


/* =========================
   PipelineSolutionStep
   ========================= */

void pipelinesolutionstep_init(PipelineSolutionStep* s, bool starts_on_inverse) {
    s->solution = malloc(sizeof(Solution));
    solution_init(s->solution);
    s->starts_on_inverse = starts_on_inverse;
}

void pipelinesolutionstep_append(PipelineSolutionStep* s, int move) {
    solution_append(s->solution, move);
}

bool pipelinesolutionstep_pop(PipelineSolutionStep* s) { return solution_pop(s->solution); }

PipelineSolutionStep pipelinesolutionstep_copy(const PipelineSolutionStep* src) {
    PipelineSolutionStep dst;
    dst.solution          = malloc(sizeof(Solution));
    *dst.solution         = solution_copy(src->solution);
    dst.starts_on_inverse = src->starts_on_inverse;
    return dst;
}

void pipelinesolutionstep_free(PipelineSolutionStep* s) {
    solution_free(s->solution);
    free(s->solution);
    s->solution          = NULL;
    s->starts_on_inverse = false;
}


/* =========================
   PipelineSolution (dynamic)
   ========================= */

void pipelinesolution_init(PipelineSolution* pipeline_solution) {
    pipeline_solution->capacity = 8;  // default capacity
    pipeline_solution->count    = 0;

    pipeline_solution->heuristic_score = -1;

    pipeline_solution->steps = malloc(sizeof(PipelineSolutionStep) * pipeline_solution->capacity);
}

void pipelinesolution_free(PipelineSolution* pipeline_solution) {
    for (int i = 0; i < pipeline_solution->count; i++)
    {
        pipelinesolutionstep_free(&pipeline_solution->steps[i]);
    }
}

PipelineSolution pipelinesolution_copy(const PipelineSolution* src) {
    PipelineSolution dst;
    dst.steps           = malloc(sizeof(PipelineSolutionStep) * src->capacity);
    dst.count           = 0;
    dst.capacity        = src->capacity;
    dst.heuristic_score = src->heuristic_score;
    for (int i = 0; i < src->count; i++)
    {
        pipelinesolution_add_copy(&dst, &src->steps[i]);
    }
    return dst;
}


int pipelinesolution_add_copy(PipelineSolution*           set,
                              const PipelineSolutionStep* pipeline_solution_step) {
    if (set->count >= set->capacity)
    {
        set->capacity *= 2;
        PipelineSolutionStep* new_steps =
          realloc(set->steps, sizeof(PipelineSolutionStep) * set->capacity);

        if (new_steps == NULL)
        {
            // Don't modify set->steps if realloc failed
            return 1;
        }
        set->steps = new_steps;  // Only update if successful
    }

    set->steps[set->count++] = pipelinesolutionstep_copy(pipeline_solution_step);
    return 0;
}

void apply_pipelinesolution(cube_t* cube, PipelineSolution* ps) {
    for (int i = 0; i < ps->count; i++)
    {
        PipelineSolutionStep pss = ps->steps[i];
        if (pss.starts_on_inverse)
        {
            *cube = cube_operation_inverse(*cube);
        }

        cube_move_apply_multiple_moves(cube, pss.solution->moves, pss.solution->length);

        if (pss.starts_on_inverse)
        {
            *cube = cube_operation_inverse(*cube);
        }
    }
}


/* =========================
   PipelineSolutionSet
   ========================= */


void pipelinesolutionset_init(PipelineSolutionSet* set, int capacity) {
    set->capacity = capacity;
    set->data     = malloc(sizeof(PipelineSolution) * capacity);
    set->count    = 0;
}

void pipelinesolutionset_free(PipelineSolutionSet* set) {
    for (int i = 0; i < set->count; i++)
    {
        pipelinesolution_free(&set->data[i]);
    }
}

int pipelinesolutionset_add_copy(PipelineSolutionSet* set, const PipelineSolution* s) {
    if (set->count >= set->capacity)
    {
        return 0;  // full
    }

    set->data[set->count++] = pipelinesolution_copy(s);
    return 1;
}


void pipelinesolutionset_append(PipelineSolutionSet* A, PipelineSolutionSet* B) {
    for (int i = 0; i < B->count; i++)
    {
        pipelinesolutionset_add_copy(A, &B->data[i]);
    }
}


void pipelinesolutionset_merge_with_prefix(
  PipelineSolutionSet*       dst,
  const PipelineSolutionSet* src,    // New solutions to append
  const PipelineSolution*    prefix  // Existing prefix to prepend
) {
    // Initialize destination with capacity for all combinations
    pipelinesolutionset_init(dst, src->count);

    for (int i = 0; i < src->count; i++)
    {
        // Create a new PipelineSolution that starts with prefix
        PipelineSolution merged;
        pipelinesolution_init(&merged);

        // Copy all prefix steps
        for (int j = 0; j < prefix->count; j++)
        {
            pipelinesolution_add_copy(&merged, &prefix->steps[j]);
        }

        // Copy src solution steps
        for (int j = 0; j < src->data[i].count; j++)
        {
            pipelinesolution_add_copy(&merged, &src->data[i].steps[j]);
        }

        // Add to destination set
        pipelinesolutionset_add_copy(dst, &merged);
        pipelinesolution_free(&merged);
    }
}


void pipelinesolutionset_compute_scores(cube_t               c,
                                        PipelineSolutionSet* next,
                                        solving_step*        next_ss) {
    for (int i = 0; i < next->count; i++)
    {
        PipelineSolution* ps = &next->data[i];

        int length = 0;
        for (int j = 0; j < ps->count; j++)
        {
            length += ps->steps[j].solution->length;
        }

        // find the heur value and default to 0 if no heur is available
        int heur = 0;
        if (next_ss != NULL && next_ss->heuristic_func != NULL && next_ss->p_data->ptable_is_loaded)
        {
            heur = next_ss->heuristic_func(&c, next_ss->p_data);
        }

        ps->heuristic_score = length + heur;
    }
}


static int pipelinesolution_cmp(const void* a, const void* b) {
    const PipelineSolution* psa = a;
    const PipelineSolution* psb = b;

    return psa->heuristic_score - psb->heuristic_score;
}

void pipelinesolutionset_trim_shortest(PipelineSolutionSet* set, int max_count) {
    if (set->count <= max_count)
        return;

    // sort by increasing score
    qsort(set->data, set->count, sizeof(PipelineSolution), pipelinesolution_cmp);

    // free the discarded solutions
    for (int i = max_count; i < set->count; i++)
    {
        pipelinesolution_free(&set->data[i]);
    }

    set->count = max_count;
}


void append_copy_solutionset_to_pipelinesolutionset(PipelineSolutionSet* pss,
                                                    SolutionSet*         ss,
                                                    bool                 starts_on_inverse) {
    for (int i = 0; i < ss->count; i++)
    {
        PipelineSolution sol;
        pipelinesolution_init(&sol);
        PipelineSolutionStep step;
        step.starts_on_inverse = starts_on_inverse;
        step.solution          = malloc(sizeof(Solution));
        *step.solution         = solution_copy(&ss->data[i]);
        pipelinesolution_add_copy(&sol, &step);
        pipelinesolutionset_add_copy(pss, &sol);
    }
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
