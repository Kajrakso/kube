#include "cli.h"

#include "solver_pipeline.h"


/* ------------------------------------ */
/* string representations and printing  */
/* ------------------------------------ */

static const char* corners_str_repr[NCORNERS] = {
  "ULB", "UBR", "URF", "UFL", "DLF", "DFR", "DRB", "DBL",
};

static const char* edges_str_repr[NEDGES] = {
  "UB", "UR", "UF", "UL", "DF", "DR", "DB", "DL", "BL", "BR", "FR", "FL",
};

char* move_notation[] = {
  "U", "U2", "U'", "D", "D2", "D'", "L", "L2", "L'",
  "R", "R2", "R'", "F", "F2", "F'", "B", "B2", "B'",
};

static inline void fill_corner_strings(cube_t* cube, char cs[NCORNERS][3]) {
    for (int i = 0; i < NCORNERS; i++)
    {
        int c = -1;

        for (int k = 0; k < NCORNERS; k++)
        {
            if (extract_corner_perm(cube->corners[k]) == i)
                c = k;
        }

        int co = extract_corner_orien(cube->corners[c], UD);

        cs[i][0] = corners_str_repr[c][(0 + 3 - co) % 3];
        cs[i][1] = corners_str_repr[c][(1 + 3 - co) % 3];
        cs[i][2] = corners_str_repr[c][(2 + 3 - co) % 3];
    }
}

static inline void fill_edge_strings(cube_t* cube, char es[NEDGES][2]) {
    for (int i = 0; i < NEDGES; i++)
    {
        int e = -1;

        for (int k = 0; k < NEDGES; k++)
        {
            if (extract_edge_perm(cube->edges[k]) == i)
                e = k;
        }

        for (int j = 0; j < 2; j++)
        {
            es[i][j] = edges_str_repr[e][(extract_edge_orien(cube->edges[e], FB) + j) % 2];
        }
    }
}

void cube_print_cube(cube_t* cube) {
    const char* cube_str_pattern = "      -------\n"
                                   "      |%c %c %c|\n"
                                   "      |%c   %c|\n"
                                   "      |%c %c %c|\n"
                                   "-------------------------\n"
                                   "|%c %c %c|%c %c %c|%c %c %c|%c %c %c|\n"
                                   "|%c   %c|%c   %c|%c   %c|%c   %c|\n"
                                   "|%c %c %c|%c %c %c|%c %c %c|%c %c %c|\n"
                                   "-------------------------\n"
                                   "      |%c %c %c|\n"
                                   "      |%c   %c|\n"
                                   "      |%c %c %c|\n"
                                   "      -------\n";
    char        cs[NCORNERS][3];
    char        es[NEDGES][2];

    fill_corner_strings(cube, cs);
    fill_edge_strings(cube, es);

    printf(cube_str_pattern, cs[0][0], es[0][0], cs[1][0], es[3][0], es[1][0], cs[3][0], es[2][0],
           cs[2][0], cs[0][1], es[3][1], cs[3][2], cs[3][1], es[2][1], cs[2][2], cs[2][1], es[1][1],
           cs[1][2], cs[1][1], es[0][1], cs[0][2], es[8][1], es[11][1], es[11][0], es[10][0],
           es[10][1], es[9][1], es[9][0], es[8][0], cs[7][2], es[7][1], cs[4][1], cs[4][2],
           es[4][1], cs[5][1], cs[5][2], es[5][1], cs[6][1], cs[6][2], es[6][1], cs[7][1], cs[4][0],
           es[4][0], cs[5][0], es[7][0], es[5][0], cs[7][0], es[6][0], cs[6][0]);
}

bool parse_move_string(Solution* result, const char* move_string) {
    // TODO: add support for rotations, wide moves, slice moves and inverse moves

    const struct {const char move; enum move value;} valid_base_moves[6] = {
      {'U', U1}, {'D', D1}, {'L', L1}, {'R', R1}, {'F', F1}, {'B', B1},
    };

    // keep track of moves both on nomal and inverse,
    // and store them in two "Solution"s.
    Solution s, s_inv;
    solution_init(&s);
    solution_init(&s_inv);

    bool is_on_inv = false;
    bool is_in_comment = false;
    
    size_t i = 0;
    while(i < strlen(move_string)){
    // for (size_t i = 0; i < strlen(move_string); i++){
        char c = move_string[i];
        char c_next = move_string[i + 1];

        if (c == '\n') {
            is_in_comment = false;
        }

        if (is_in_comment || c == ' ' || c == '\n' || c == '\t' || c == '\r'){
            i += 1;
            continue;
        }


        if (c == '(') {
            if (is_on_inv) {
                fprintf(stderr, "Parsing error: Found ( following a (\n");
                solution_free(&s); solution_free(&s_inv);
                return false;
            }
            is_on_inv = true;
            i += 1;
            continue;
        }

        if (c == ')') {
            if (!is_on_inv) {
                fprintf(stderr, "Parsing error: Found ) without matching (\n");
                solution_free(&s); solution_free(&s_inv);
                return false;
            }
            is_on_inv = false;
            i += 1;
            continue;
        }

        if (c == '/') {
            is_in_comment = true;
            i += 1;
            continue;
        }

        bool valid_move = false;
        for (size_t j = 0; j < 6; j += 1) {
            if (c == valid_base_moves[j].move) {
                int move = valid_base_moves[j].value;

                // check next char also!
                if (c_next == '1'){
                    move += 0;
                    i += 1;
                }
                if (c_next == '2'){
                    move += 1;
                    i += 1;
                }
                if (c_next == '\'' || c_next == '3'){
                    move += 2;
                    i += 1;
                }

                valid_move = true;
                solution_append(is_on_inv ? &s_inv : &s, move);
                i += 1;
                break;
            }
        }

        if (!valid_move) {
            fprintf(stderr, "Parsing error: Invalid move: %c\n", c);
            solution_free(&s);
            solution_free(&s_inv);
            return false;
        }


    }

    if (is_on_inv) {
        // then ( was not closed, raise!
        fprintf(stderr, "Parsing error: ( was not closed\n");
        solution_free(&s);
        solution_free(&s_inv);
        return false;
    }

    // for a scramble we do moves on inverse as premoves
    *result = solution_merge_inverse_and_normal(&s_inv, &s);

    // Clean up and set output length
    solution_free(&s);
    solution_free(&s_inv);

    return true;
}

void cube_print_solutions(int* solutions, int num_sols, int verbose) {
    for (int sol = 0; sol < num_sols; sol++)
    {
        int len = 0;
        for (int m = 0; m < 20; m++)
        {
            int move = solutions[20 * sol + m];
            if (is_valid_move(move))
            {
                printf("%s ", move_notation[move]);
                len++;
            }
        }
        printf("(%i)\n", len);
    }
}


void cube_print_solution_set(SolutionSet* solution_set, int verbose) {
    size_t    count     = solution_set->count;
    Solution* solutions = solution_set->data;
    for (size_t solution_idx = 0; solution_idx < count; solution_idx++)
    {
        Solution solution = solutions[solution_idx];
        for (size_t move_idx = 0; move_idx < solution.length; move_idx++)
        {
            int move = solution.moves[move_idx];
            if (is_valid_move(move))
            {
                printf("%s ", move_notation[move]);
            }
            else
            {
                printf("? ");
            }
        }
        printf("(%lu)\n", solution.length);
    }
}


void cube_print_pipelinesolution(PipelineSolution* ps, int verbose){
    char* step_separator = "\n";
    // Print each step in the pipeline solution
    for (size_t step_idx = 0; step_idx < ps->count; step_idx++)
    {
        PipelineSolutionStep* step = &ps->steps[step_idx];

        // Optionally indicate if step starts on inverse
        if (step->starts_on_inverse)
        {
            printf("(");
        }

        // Print the moves for this step
        for (size_t move_idx = 0; move_idx < step->solution->length; move_idx++)
        {
            int move = step->solution->moves[move_idx];
            if (is_valid_move(move))
            {
                printf("%s", move_notation[move]);
                if (move_idx < step->solution->length - 1)
                {
                    printf(" ");
                }
            }
            else
            {
                printf("?");
            }
        }

        if (step->starts_on_inverse)
        {
            printf(")");
        }

        // Print step separator
        if (ps->count > 0)
        {
            printf("%s", step_separator);
        }
    }

    // Print verbose information
    if (verbose == 1)
    {
        size_t total_moves = 0;
        for (size_t step_idx = 0; step_idx < ps->count; step_idx++)
        {
            total_moves += ps->steps[step_idx].solution->length;
        }
        printf(" (total: %lu moves, %lu steps", total_moves, ps->count);

        if (ps->heuristic_score != SIZE_MAX)
        {
            printf(", score: %lu", ps->heuristic_score);
        }
        printf(")");
    }
}

void cube_print_pipelinesolution_set(PipelineSolutionSet* p, int verbose) {
    for (size_t sol_idx = 0; sol_idx < p->count; sol_idx++)
    {
        PipelineSolution* ps = &p->data[sol_idx];

        // Print solution number if there are multiple solutions
        if (p->count > 1)
        {
            printf("Solution %lu:\n", sol_idx + 1);
        }
        cube_print_pipelinesolution(ps, verbose);

        // Add blank line between solutions for readability
        if (p->count > 0 && sol_idx < p->count - 1)
        {
            printf("\n");
        }
    }
}


/* ----------------------- */
/* arg parser */
/* ----------------------- */


/* Parse a single option. */
error_t parse_opt(int key, char* arg, struct argp_state* state) {
    /* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
    struct arguments* arguments = state->input;

    // for parsing number of solutions
    char* endptr;
    long  num;
    long  depth_limit;

    switch (key)
    {
    case 'g' :
        arguments->gen = 1;
        break;
    case 'i' :
        arguments->stdin_mode = 1;
        break;

    case 'v' :
        arguments->verbose = 1;
        break;

    case 'f' :
        arguments->format = arg;
        break;

    case 't' :
        num = strtol(arg, &endptr, 10);

        if (*endptr != '\0')
        {
            // Error: not a valid integer string
            printf("Conversion error, non-integer characters found: %s. Using number_of_threads = %i\n", endptr, arguments->number_of_threads);
        }
        else
        {
            arguments->number_of_threads = (int)num;
        }
        break;

    case 's' :
        if (arguments->step_count >= MAX_STEPS)
            argp_error(state, "Too many --step options");

        struct step* st = &arguments->steps[arguments->step_count++];
        st->max_depth   = -1;  // default

        // parse "eo:max=7,metric=htm"
        char* spec = strdup(arg);
        char* tok  = strtok(spec, ":");

        st->name = tok;

        tok = strtok(NULL, ",");
        while (tok)
        {
            // we do not support extra options currently
            //
            // if (strncmp(tok, "max=", 4) == 0)
            //     st->max_depth = atoi(tok + 4);
            // else if (strncmp(tok, "metric=", 7) == 0)
            //     st->metric = tok + 7;
            // else
            argp_error(state, "Unknown step option: %s", tok);

            tok = strtok(NULL, ",");
        }
        break;

    case 'n' :
        num = strtol(arg, &endptr, 10);

        if (*endptr != '\0')
        {
            // Error: not a valid integer string
            printf("Conversion error, non-integer characters found: %s. Using n = %i\n", endptr, 1);
        }
        else
        {
            arguments->number_of_solutions = (int)num;
        }
        break;

    case 'M' :
        depth_limit = strtol(arg, &endptr, 10);

        if (*endptr != '\0')
        {
            // Error: not a valid integer string
            printf("Conversion error, non-integer characters found: %s. Using m = %i\n", endptr, 1);
        }
        else
        {
            arguments->depth_limit = (int)depth_limit;
        }
        break;
    case ARGP_KEY_ARG:
        arguments->scramble = strdup(arg);
        break;

    default :
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}


void set_default_values_arguments(struct arguments* arguments) {
    /* Default values. */
    arguments->verbose = 0;
    arguments->stdin_mode = 0;
    arguments->scramble = "";
    arguments->gen     = 0;
    arguments->format  = "singmaster";
    // arguments->steps[0]            = (struct step){.name = "fin", .max_depth = -1};
    arguments->step_count          = 0;
    arguments->number_of_solutions = 1;
    arguments->depth_limit = 1024;  // practically inifinite
    
    long n = sysconf(_SC_NPROCESSORS_ONLN);
    arguments->number_of_threads = n > 0 ? (int)n : 1;
}


void cli_gen() {
    printf("Starting to gen tables...\n");
    char fname[strlen(tabledir) + FILENAME_MAX];

    // this is needed since kube currently
    // does not generate this file itself
    strcpy(fname, tabledir);
    strcat(fname, "/");
    printf(
        "TEMP: If you want to solve to HTR you need to copy dr_subsets.dat to this location: %s\n",
        fname);

    struct timespec start, end;
    timespec_get(&start, TIME_UTC);
    cube_tables_generate();

    char fname1[strlen(tabledir) + FILENAME_MAX];
    strcpy(fname1, tabledir);
    strcat(fname1, "/");
    strcat(fname1, "sym_table_e_index.dat");


    if (file_exists(fname1))
    {
        fprintf(stderr, "%s already exists. I'm skipping it!\n", fname1);
    }
    else
    {
        gen_sym_table_e_index();
    }


    for (int i = 0; i < 2; i++)
    {
        char fname2[strlen(tabledir) + FILENAME_MAX];
        strcpy(fname2, tabledir);
        strcat(fname2, "/");
        strcat(fname2, enabled_ptables[i]->filename);

        if (file_exists(fname2))
        {
            fprintf(stderr, "%s already exists. I'm skipping it!\n", fname2);
        }
        else
        {
            enabled_ptables[i]->gen_ptable_func();
        }
    }

    timespec_get(&end, TIME_UTC);
    double elapsed = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Total time used for table gen (in seconds): %f\n", elapsed);
}


int cli_solver_prepare(struct arguments arguments, solving_step** steps){
    cube_tables_generate();  // generates tables for moves, symmetries, etc.

    // load all tables needed for all the steps.
    for (int i = 0; i < arguments.step_count; i++)
    {
        struct step s = arguments.steps[i];

        solving_step* ss = NULL;
        if (strcmp(s.name, "fin") == 0)
        {
            ss = &fin;
        }
        if (strcmp(s.name, "dr") == 0)
        {
            ss = &dr;
        }
        if (strcmp(s.name, "eo") == 0)
        {
            ss = &eo;
        }
        if (strcmp(s.name, "htr") == 0)
        {
            ss = &htr;
        }

        if (ss == NULL)
        {
            printf("Did not understand step. exiting...\n");
            return 1;
        }

        if (ss->p_data == NULL)
        {
            if (arguments.verbose == 1)
                fprintf(stderr, "\tstep %s aint got ptable!\n", s.name);
        }
        else if (cube_tables_load_ptable(ss->p_data) == 1)
        {
            fprintf(stderr, "\tstep %s got ptable but ", s.name);
            fprintf(stderr, "\tcould not load ptable! Trying to solve step: %i\n",
                    ss->solving_type);
        }

        // load some special tables needed for some of the steps
        if (ss->solving_type == SOLVE_FIN)
        {
            cube_tables_load_sym_table_e_index();
        }

        if (ss->solving_type == SOLVE_HTR)
        {
            cube_tables_load_dr_subsets();
        }

        steps[i] = ss;
    }

    return 0;
}

void cli_solver_cleanup(struct arguments arguments, solving_step** steps){
    cube_tables_free();
    for (int i = 0; i < arguments.step_count; i++)
    {
        solving_step* ss = steps[i];
        if (ss->p_data != NULL)
        {
            free_ptable(ss->p_data);
        }
    }   
}

int solve(char* scr, struct arguments arguments, solving_step** steps){
    cube_t c = cube_create_new_cube();
    if (cube_scrambler_scramble_cube(&c, scr, arguments.format) != 0){
        fprintf(stderr, "Error reading scramble.\n");
        return 1;
    };


    struct timespec start, end;
    timespec_get(&start, TIME_UTC);

    if (arguments.step_count == 1 || arguments.number_of_solutions == 1)
    {
        // we invoke a simple pipeline solver:
        solver_pipeline(c, arguments, steps);
    }
    else
    {
        // we invoke a beam search since we have multiple steps and multiple solutions
        solver_beam_search(c, arguments, steps);
    }
    timespec_get(&end, TIME_UTC);
    if (arguments.verbose) {
        double elapsed = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1e9;
        printf("Time used (in seconds): %f\n", elapsed);
    }

    return 0;
}

void cli_solver_solve(struct arguments arguments, solving_step** steps){
    solve(arguments.scramble, arguments, steps);
}

void cli_solver_solving_loop(struct arguments arguments, solving_step** steps){
    char* buf = malloc(BUF_SIZE);
    while (fgets(buf, BUF_SIZE, stdin))
    {
        buf[strcspn(buf, "\r\n")] = 0;

        if (solve(buf, arguments, steps) != 0) {
            free(buf);
            return;
        }

    }
    free(buf);
}
