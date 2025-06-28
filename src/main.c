#include <stdlib.h>
#include <argp.h>

#include "solver.h"
#include "cli.h"
#include "scrambler.h"
#include "env.h"

#define BUF_SIZE 4096

const char* argp_program_version     = "kube 0.1";
const char* argp_program_bug_address = "<kube@oskarfj.no>";

/* Program documentation. */
static char doc[] = "kube -- an optimal Rubik's cube solver";

/* A description of the arguments we accept. */
static char args_doc[] = "";

/* The options we understand. */
static struct argp_option options[] = {{"verbose", 'v', 0, 0, "Produce verbose output"},
                                       {"num", 'n', "NUM", 0, "Try to find NUM solutions"},
                                       {"format", 'f', "FORMAT", 0, "Specify scramble format"},
                                       {"gen", 'g', 0, 0, "Generate tables"},
                                       {0}};

/* Used by main to communicate with parse_opt. */
struct arguments {
    char* format;
    int   verbose;
    int   gen;
    int   number_of_solutions;
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

    /* Parse our arguments; every option seen by parse_opt will
     be reflected in arguments. */
    argp_parse(&argp, argc, argv, 0, 0, &arguments);
    init_env();

    if (arguments.gen == 1)
    {
        cube_tables_generate();

        char        fname1[strlen(tabledir) + 100];
        char        fname2[strlen(tabledir) + 100];

        strcpy(fname1, tabledir);
        strcat(fname1, "/");
        strcat(fname1, "sym_table_e_index.dat");

        strcpy(fname2, tabledir);
        strcat(fname2, "/");
        strcat(fname2, "H.dat");

        if (file_exists(fname1))
        {
            fprintf(stderr, "%s already exists. I'm skipping it!\n", fname1);
        }
        else
        {
            gen_sym_table_e_index();
        }

        if (file_exists(fname2))
        {
            fprintf(stderr, "%s already exists. I'm skipping it!\n", fname2);
        }
        else
        {
            gen_ptable_H();
        }
        return 0;
    }

    if (arguments.number_of_solutions >= 1)
    {
        cube_tables_generate();  // generates tables for moves, symmetries, etc.
        cube_tables_load();      // loads the pruning tables

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
                                        arguments.verbose))
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
