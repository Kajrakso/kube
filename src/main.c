#include <stdlib.h>
#include <argp.h>
#include <time.h>

#include "solver.h"
#include "cli.h"
#include "scrambler.h"
#include "env.h"

#include "tables_ptable_data.h"

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
                                       {"ptable_test", 't', 0, 0, "use testing ptable"},
                                       {0}};

/* Used by main to communicate with parse_opt. */
struct arguments {
    char* format;
    int   verbose;
    int   gen;
    int   number_of_solutions;
    int  ptable_test;
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

    case 't' :
        arguments->ptable_test = 1;
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
    arguments.number_of_solutions = 1;
    arguments.ptable_test         = 0;

    /* Parse our arguments; every option seen by parse_opt will
     be reflected in arguments. */
    argp_parse(&argp, argc, argv, 0, 0, &arguments);
    init_env();

    if (arguments.gen == 1)
    {
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
            char        fname2[strlen(tabledir) + FILENAME_MAX];
            strcpy(fname2, tabledir);
            strcat(fname2, "/");
            strcat(fname2, enabled_ptables[i]->filename);

            if (file_exists(fname2))
            {
                fprintf(stderr, "%s already exists. I'm skipping it!\n", fname2);
            }
            else
            {
                clock_t start, end;
                start = clock();
                enabled_ptables[i]->gen_ptable_func();
                end = clock();
                printf("Total time used for table gen: %f s\n", (float)(end - start) / CLOCKS_PER_SEC);
            }
        }

        cube_tables_load_ptable(&ptable_data_dr);
        analyze_ptable(ptable_data_dr);
        free_ptable(&ptable_data_dr);

        return 0;
    }

    if (arguments.number_of_solutions >= 1)
    {
        cube_tables_generate();  // generates tables for moves, symmetries, etc.

        ptable_data_t p_data = arguments.ptable_test
            ? *enabled_ptables[1]
            : *enabled_ptables[0];

        cube_tables_load_ptable(&p_data);
        cube_tables_load_sym_table_e_index();

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

            if (cube_solvers_solve_cube(c, solutions, arguments.number_of_solutions,
                                        arguments.verbose, p_data))
            {
                cube_print_solutions(solutions, arguments.number_of_solutions, arguments.verbose);
            }
            else
            {
                printf("Could not solve the cube :(\n");
            }
        }

        cube_tables_free();
        free(buf);
        free(solutions);
    }

    return 0;
}
