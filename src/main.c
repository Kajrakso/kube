#include <stdlib.h>
#include <argp.h>
#include <time.h>

#include "solver.h"
#include "cli.h"
#include "scrambler.h"
#include "env.h"

#include "tables_ptable_data.h"
#include "solver_steps.h"

#define BUF_SIZE 4096

const char* argp_program_version     = "kube 0.1";
const char* argp_program_bug_address = "<oskarfj@oskarfj.no>";

/* Program documentation. */
static char doc[] = "kube -- an optimal Rubik's cube solver";

/* A description of the arguments we accept. */
static char args_doc[] = "";

/* The options we understand. */
static struct argp_option options[] = {{"verbose", 'v', 0, 0, "Produce verbose output"},
                                       {"num", 'n', "NUM", 0, "Try to find NUM solutions"},
                                       {"format", 'f', "FORMAT", 0, "Specify scramble format"},
                                       {"gen", 'g', 0, 0, "Generate tables"},
                                       {"step", 's', "STEP", 0,
                                        "Append a solving step (ordered). Can be repeated.\n"
                                        "Examples:\n"
                                        "  -s eo:max=7 -s dr:max=10 -s fin"},
                                       {0}};
#define MAX_STEPS 32

struct step {
    char *name;
    int max_depth;
};


/* Used by main to communicate with parse_opt. */
struct arguments {
    char* format;
    int   verbose;
    int   gen;
    int   number_of_solutions;

    struct step steps[MAX_STEPS];
    int step_count;
};

/* Parse a single option. */
static error_t parse_opt(int key, char* arg, struct argp_state* state) {
    /* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
    struct arguments* arguments = state->input;

    // for parsing number of solutions
    char* endptr;
    long  num;

    switch (key)
    {
    case 'g' :
        arguments->gen = 1;
        break;

    case 'v' :
        arguments->verbose = 1;
        break;

    case 'f' :
        arguments->format = arg;
        break;

    case 's':
        if (arguments->step_count >= MAX_STEPS)
            argp_error(state, "Too many --step options");

        struct step *st = &arguments->steps[arguments->step_count++];
        st->max_depth = -1;   // default

        // parse "eo:max=7,metric=htm"
        char *spec = strdup(arg);
        char *tok = strtok(spec, ":");

        st->name = tok;

        tok = strtok(NULL, ",");
        while (tok) {
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
            arguments->number_of_solutions = num;
        }
        break;

    default :
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

/* Our argp parser. */
static struct argp argp = {options, parse_opt, args_doc, doc};

int main(int argc, char** argv) {
    struct arguments arguments;

    /* Default values. */
    arguments.verbose             = 0;
    arguments.gen                 = 0;
    arguments.format              = "singmaster";
    // arguments.steps[0]            = (struct step){.name = "fin", .max_depth = -1};
    arguments.step_count          = 0;
    arguments.number_of_solutions = 1;

    /* Parse our arguments; every option seen by parse_opt will
     be reflected in arguments. */
    argp_parse(&argp, argc, argv, 0, 0, &arguments);
    init_env();

    if (arguments.gen == 1)
    {
        clock_t start, end;
        start = clock();
        cube_tables_generate();

        char        fname1[strlen(tabledir) + FILENAME_MAX];
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


        for (int i = 0; i < 2; i++){
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

        end = clock();
        printf("Total time used for table gen: %f s\n", (float)(end - start) / CLOCKS_PER_SEC);
        // cube_tables_load_ptable(&ptable_data_dr);
        // analyze_ptable(ptable_data_dr);
        // free_ptable(&ptable_data_dr);

        return 0;
    }


    if (arguments.number_of_solutions > 1 && arguments.step_count > 1){
        fprintf(stderr, "-n is not supported with multiple steps. Uses n = 1.");
        arguments.number_of_solutions = 1;
    }

    if (arguments.number_of_solutions >= 1)
    {
        cube_tables_generate();  // generates tables for moves, symmetries, etc.
        solving_step* steps[arguments.step_count];

        // load all tables needed for all the steps.
        for (int i = 0; i < arguments.step_count; i++){
            struct step s = arguments.steps[i];

            solving_step* ss = NULL;
            if (strcmp(s.name, "fin") == 0){
                ss = &fin;
            }
            if (strcmp(s.name, "dr") == 0){
                ss = &dr;
            }
            if (strcmp(s.name, "eo") == 0){
                ss = &eo;
            }
            if (strcmp(s.name, "htr") == 0){
                ss = &htr;
            }
            // if (strcmp(s.name, "cross") == 0){
            //     ss = &cross_D;
            // }
            // if (strcmp(s.name, "xcross") == 0){
            //     ss = &xcross_D;
            // }
            // if (strcmp(s.name, "xxcross") == 0){
            //     ss = &xxcross_D;
            // }
            // if (strcmp(s.name, "xxxcross") == 0){
            //     ss = &xxxcross_D;
            // }
            // if (strcmp(s.name, "xxxxcross") == 0){
            //     ss = &xxxxcross_D;
            // }

            if (ss == NULL){
                printf("Did not understand step. exiting...\n");
                return 1;
            }

            if (ss->p_data == NULL){
                fprintf(stderr, "\tstep %s aint got ptable!\n", s.name);
            }
            else if (cube_tables_load_ptable(ss->p_data) == 1){
                fprintf(stderr, "\tstep %s got ptable but ", s.name);
                fprintf(stderr, "\tcould not load ptable! Trying to solve step: %i\n", ss->solving_type);
            }

            // load some special tables needed for some of the steps
            if (ss->solving_type == SOLVE_FIN){
                cube_tables_load_sym_table_e_index();
            }

            if (ss->solving_type == SOLVE_HTR || ss->solving_type == SOLVE_DR){
                cube_tables_load_dr_subsets();
            }

            steps[i] = ss;
        }

        char* buf       = malloc(BUF_SIZE);
        int*  solutions = malloc(sizeof(int) * 20 * arguments.number_of_solutions);

        while (fgets(buf, BUF_SIZE, stdin))
        {
            buf[strcspn(buf, "\r\n")] = 0;

            cube_t c = cube_create_new_cube();
            if (strcmp(arguments.format, "speffz") == 0)
            {
                if (cube_scrambler_parse_speffz(&c, buf) == 1)
                {
                    break;
                }
            }
            else if (strcmp(arguments.format, "singmaster") == 0)
            {
                if (cube_move_apply_move_string(&c, buf) == 1)
                {
                    break;
                }
            }

            
            for (int i = 0; i < arguments.step_count; i++){
                solving_step* ss = steps[i];

                // not sure if this continues on the previous solve or if it uses the same scramble?
                if (cube_solvers_solve_cube(c, solutions, arguments.number_of_solutions,
                                            arguments.verbose, ss))
                {
                    cube_print_solutions(solutions, arguments.number_of_solutions, arguments.verbose);

                    // we do pipeline mode if user has supplied multiple steps
                    if (arguments.step_count > 1){
                        for (int i = 0; i < 20; i++){
                            if (solutions[i] != -1) {
                                cube_move_apply_move(&c, solutions[i]);
                            }
                            else {
                                break;
                            }
                        }
                    }
                }
                else
                {
                    printf("Could not solve the cube :(\n");
                }
            }


        }

        // todo: make cleaning up tables easier
        cube_tables_free();
        for (int i = 0; i < arguments.step_count; i++){
            solving_step* ss = steps[i];
            if (ss->p_data != NULL){
                free_ptable(ss->p_data);
            }
        }
        // free_ptable(p_data);
        free(buf);
        free(solutions);
    }

    return 0;
}
