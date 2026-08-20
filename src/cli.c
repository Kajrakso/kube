#include "cli.h"
#include "solver_pipeline.h"

#include "moveset.h"
#include "dsl.h"
#include "dsl_compile.h"
#include "dsl_prune.h"

/* ------------------------------------ */
/* global variables for the -D flag     */
/* ------------------------------------ */

dsl_compile_definition_t* custom_solved_definitions = NULL;
int custom_solved_definitions_count = 0;

/* ------------------------------------------ */
/* functions related to handeling the -D flag */
/* ------------------------------------------ */

static bool valid_identifier(const char* s) {
    if (!(isalpha((unsigned char)s[0]) || s[0] == '_')) return false;
    for (const char* c = s + 1; *c; c++)
        if (!(isalnum((unsigned char)*c) || *c == '_')) return false;
    return true;
}

static bool is_reserved_definition_name(const char* s) {
    for (size_t i = 0; i < sizeof(dsl_primitives) / sizeof(dsl_primitives[0]); i++) {
        if (strcmp(s, dsl_primitives[i].name) == 0) {
            return true;
        }
    }
    return false;
}

static int definition_index(const struct arguments* arguments, const char* name) {
    for (int i = 0; i < arguments->def_count; i++)
        if (strcmp(arguments->defs[i].name, name) == 0) return i;
    return -1;
}


/* ------------------------------------ */
/* string representations               */
/* ------------------------------------ */

static const char* corners_str_repr[NCORNERS] = {
  "ULB", "UBR", "URF", "UFL", "DLF", "DFR", "DRB", "DBL",
};

static const char* edges_str_repr[NEDGES] = {
  "UB", "UR", "UF", "UL", "DF", "DR", "DB", "DL", "BL", "BR", "FR", "FL",
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


/* --------------------------------------- */
/*                   printing              */
/* --------------------------------------- */

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


/* ------------------------------ */
/*           parsing              */
/* ------------------------------ */

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
    size_t len_move_string = strlen(move_string);
    while(i < len_move_string){
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
    char* spec;
    long  num;
    // long  depth_limit;

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
        st->number_of_solutions = 1; // default

        // parse "eo:max=7,num=15"
        spec = strdup(arg);
        char* tok  = strtok(spec, ":");

        st->name = tok;

        tok = strtok(NULL, ",");
        while (tok)
        {
            // we do not support extra options currently
            if (strncmp(tok, "max=", 4) == 0)
                st->max_depth = atoi(tok + 4);
            else if (strncmp(tok, "num=", 4) == 0)
                st->number_of_solutions = atoi(tok + 4);
            else {
                free(spec);
                argp_error(state, "Unknown step option: %s", tok);
            }

            tok = strtok(NULL, ",");
        }
        break;

    case 'D':
        if (arguments->def_count >= MAX_DEFS){
            argp_error(state, "Too many --define options"); 
        }

        spec = strdup(arg);
        char* eq = strchr(spec, '=');

        if (!eq) {
            free(spec);
            argp_error(state, "--define require NAME=EXPR");
        }
        *eq = '\0'; 

        if (!valid_identifier(spec) || is_reserved_definition_name(spec)) {
            free(spec);
            argp_error(state, "invalid step name in --define");
        }

        /* Optional moveset suffix: NAME=EXPR@MOVESET */
        uint32_t moveset_mask = MOVESET_HTM; /* default to all 18 moves in HTM. */
        char* at = strchr(eq + 1, '@');
        if (at) {
            *at = '\0';
            char err[128];
            if (moveset_parse(at + 1, &moveset_mask, err, sizeof err) != 0) {
                fprintf(stderr, "invalid moveset: %s\n", err);
                free(spec);
                argp_error(state, "invalid moveset");
            }
        }

        struct dsl_def* d = &arguments->defs[arguments->def_count++];
        d->name = strdup(spec);
        d->expr = strdup(eq + 1);
        d->moveset_mask = moveset_mask;
        free(spec);
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
    arguments->verbose          = 0;
    arguments->stdin_mode       = 0;
    arguments->scramble         = "";
    arguments->gen              = 0;
    arguments->format           = "singmaster";
    arguments->step_count       = 0;
    arguments->def_count        = 0;
    
    long n = sysconf(_SC_NPROCESSORS_ONLN);
    arguments->number_of_threads = n > 0 ? (int)n : 1;
}


void cli_gen(struct arguments arguments) {
    printf("Starting to gen tables...\n");

    // tabledir location
    char fname[strlen(tabledir) + FILENAME_MAX];
    strcpy(fname, tabledir);
    strcat(fname, "/");

    // this is needed since kube currently
    // does not generate this file itself
    printf(
        "TEMP: If you want to solve to HTR you need to copy dr_subsets.dat to this location: %s\n",
        fname);

    struct timespec start, end;
    timespec_get(&start, TIME_UTC);
    cube_tables_generate();

    /* TODO: This is kinda ugly. Fix. */
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


    /* TODO: Should this be here? Do we want to save these to disk? */
    // if (dsl_compile_build_definitions(&custom_solved_definitions, &custom_solved_definitions_count, &arguments) == 0) {
    //     // gen ptables and save to disk 
    // }
    


    timespec_get(&end, TIME_UTC);
    double elapsed = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Total time used for table gen (in seconds): %f\n", elapsed);
}

void cli_gen_custom_ptables(struct arguments* arguments, struct step* s, solving_step* ss);

int cli_solver_prepare(struct arguments arguments, solving_step** steps){
    cube_tables_generate();  // generates tables for moves, symmetries, etc.


    if (dsl_compile_build_definitions(&custom_solved_definitions, &custom_solved_definitions_count, &arguments) != 0) {
        return 1;
    }

    // load all tables needed for all the steps.
    for (int i = 0; i < arguments.step_count; i++)
    {
        struct step s = arguments.steps[i];
        solving_step* ss = NULL;

        // TODO: check these also against predefined steps
        if      (strcmp(s.name, "fin")  == 0)    ss = &fin;
        else if (strcmp(s.name, "dr")   == 0)    ss = &dr;
        else if (strcmp(s.name, "eo")   == 0)    ss = &eo;
        else if (strcmp(s.name, "htr")  == 0)    ss = &htr;
        else {
            int def_index = definition_index(&arguments, s.name);
            if (def_index < 0) {
                // TODO: fins all available steps programatically
                fprintf(stderr, "Unknown step '%s'. Available: fin, dr, eo, htr", s.name);
                for (int k = 0; k < arguments.def_count; k++) {
                    fprintf(stderr, ", %s", arguments.defs[k].name);
                }
                fprintf(stderr, "\n");
                return 1;
            }
            custom_solved_definitions[def_index].referenced = true;
            ss = &custom_solved_definitions[def_index].step;
        }

        if (ss == NULL)
        {
            printf("Did not understand step. exiting...\n");
            return 1;
        }


        if (ss->custom_ptables != NULL && ss->n_custom_ptables > 0)
        {
            cli_gen_custom_ptables(&arguments, &s, ss);
        }
        else if (ss->p_data == NULL) {
            if (arguments.verbose == 1) {
                fprintf(stderr, "\tstep %s aint got ptable!\n", s.name);
            }
        }
        else if (cube_tables_load_ptable(ss->p_data) == 1)
        {
            fprintf(stderr, "\tstep %s got ptable but ", s.name);
            fprintf(stderr, "\tcould not load ptable! Trying to solve step: %i\n",
                    ss->solving_type);
        }

        // load some special tables needed for some of the steps
        if (ss->p_data == &ptable_data_opt1 ||
            (ss->p_data != NULL &&
             ss->p_data->cube_to_index_func == ptable_data_opt1.cube_to_index_func))
        {
            cube_tables_load_sym_table_e_index();
        }

        if (ss->solving_type == SOLVE_HTR)
        {
            cube_tables_load_dr_subsets();
        }

        steps[i] = ss;
    }


    /* warn-and-ignore unreferenced definitions */
    for (int i = 0; i < custom_solved_definitions_count; i++) {
        if (!custom_solved_definitions[i].referenced) {
            fprintf(stderr, "note: definition '%s' is not used by any -s step (ignored)\n",
                    custom_solved_definitions[i].name);
        }
    }


    return 0;
}




/* -------------------------------------------- */
/*                    cli                       */
/* -------------------------------------------- */

void cli_solver_cleanup(struct arguments arguments, solving_step** steps){
    cube_tables_free();
    for (int i = 0; i < arguments.step_count; i++)
    {
        solving_step* ss = steps[i];

        /* Custom step tables are freed by dsl_prune_free_tables
         * via dsl_compile_free_definitions. Skip them here. */
        if (ss->is_custom) {
            continue;
        }

        if (ss->p_data != NULL) {
            free_ptable(ss->p_data);
        }
    }   

    dsl_compile_free_definitions(custom_solved_definitions, &custom_solved_definitions_count);
}

int solve(char* scr, struct arguments arguments, solving_step** steps){
    cube_t c = cube_create_new_cube();
    if (cube_scrambler_scramble_cube(&c, scr, arguments.format) != 0){
        fprintf(stderr, "Error reading scramble.\n");
        return 1;
    };


    struct timespec start, end;
    timespec_get(&start, TIME_UTC);

    if (arguments.step_count == 1)
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



// TOOD: move this to dsl_*.c?
void cli_gen_custom_ptables(struct arguments* arguments, struct step* s, solving_step* ss){
    bool all_loaded = true;

    for (int k = 0; k < ss->n_custom_ptables; k++) {
        ptable_data_t* table = ss->custom_ptables[k];
        if (table->ptable_is_loaded) continue;

        /* Check if ptable is loadable (from a previous generation maybe?) */
        if (cube_tables_load_ptable(table) == 0) {
            fprintf(stderr, "Load custom ptable '%s' from disk.\n",
                    table->name);
            continue;
        }

        custom_prune_table_ctx_t* ctx =
            (custom_prune_table_ctx_t*)table->custom_data;
        /* for table 0, custom_data is dsl_prune_step_ctx_t,
         * not custom_prune_table_ctx_t. Read from table_ctxs instead. */
        if (k == 0) {
            dsl_prune_step_ctx_t* sc = (dsl_prune_step_ctx_t*)table->custom_data;
            ctx = &sc->table_ctxs[0];
        }

        uint64_t (*init_fn)(ptable_gen_ctx_t*, cube_t*, uint64_t*) =
            (ctx->kind == PRUNE_EDGES)
            ? dsl_prune_init_edges
            : dsl_prune_init_corners;
        void (*decompose_fn)(ptable_gen_ctx_t*, uint64_t, uint64_t*) =
            (ctx->kind == PRUNE_EDGES)
            ? dsl_prune_decompose_edges
            : dsl_prune_decompose_corners;

        fprintf(stderr, "Generating custom ptable '%s' (%llu states)...\n",
                table->name, (unsigned long long)table->number_of_elements);

        /* Temporarily give table 0 a per-table ctx in custom_data
         * so init/decompose see custom_prune_table_ctx_t*, not step_ctx. */
        void* saved_custom_data = NULL;
        if (k == 0) {
            saved_custom_data = table->custom_data;
            table->custom_data = ctx;
        }

        ptable_gen_ctx_t gen_ctx = {
            .ptable_data = table,
            .num_components = 3,
            .dls_max_depth = 8,                /* USER DECIDES THIS */
            .nbhr_min_depth = 8,               /* must == dls_max_depth */
            .nbhr_max_depth_excl = 8,          /* must == dls_max_depth */
            .apply_move = NULL,
            .init = init_fn,
            .setup = NULL,
            .decompose_index = decompose_fn,
        };

        table->moveset_mask = moveset_eff(ss->moveset_mask);

        /* Actually invoke the genner */
        table_prune_gen(&gen_ctx);

        if (k == 0) {
            table->custom_data = saved_custom_data;
        }

        /* table_prune_gen saved to disk and freed the buffer.
         * Load it back via mmap. */
        if (cube_tables_load_ptable(table) != 0) {
            fprintf(stderr, "Failed to load custom ptable '%s' after gen.\n",
                    table->name);
            all_loaded = false;
        }
       
        if (arguments->verbose == 1){
            tables_prune_print_ptable_data_t(table, stdout);
            printf("Analyzing table content...\n");
            analyze_ptable(*table);
        }
    }

    if (!all_loaded)
    {
        fprintf(stderr, "\tstep %s got ptables but ", s->name);
        fprintf(stderr, "\tcould not load ptable! Trying to solve step: %i\n",
                ss->solving_type);
    }
}
