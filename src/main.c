#include "solver.h"
#include "cli.h"
#include "env.h"

#include <stdlib.h>
#include <argp.h>

static int number_of_solutions = 1;

const char* argp_program_version     = "kube 0.1";
const char* argp_program_bug_address = "<kube@oskarfj.no>";

/* Program documentation. */
static char doc[] = "kube -- an optimal Rubik's cube solver";

/* A description of the arguments we accept. */
static char args_doc[] = "scramble";

/* The options we understand. */
static struct argp_option options[] = {{"verbose", 'v', 0, 0, "Produce verbose output"},
                                       {0, 'n', "NUM", 0, "Try to find NUM solutions"},
                                       {0}};

/* Used by main to communicate with parse_opt. */
struct arguments {
    char* args[1]; /* scramble */
    int   verbose;
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
    case 'v' :
        arguments->verbose = 1;
        break;

    case 'n' :
        num = strtol(arg, &endptr, 10);

        if (*endptr != '\0')
        {
            // Error: not a valid integer string
            printf("Conversion error, non-integer characters found: %s. Using n = %i\n", endptr,
                   number_of_solutions);
        }
        else
        {
            number_of_solutions = num;
        }
        break;

    case ARGP_KEY_ARG :
        if (state->arg_num >= 1)
            /* Too many arguments. */
            argp_usage(state);

        arguments->args[state->arg_num] = arg;

        break;

    case ARGP_KEY_END :
        if (state->arg_num < 1)
            /* Not enough arguments. */
            argp_usage(state);
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
    arguments.number_of_solutions = number_of_solutions;

    /* Parse our arguments; every option seen by parse_opt will
     be reflected in arguments. */
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    if (arguments.number_of_solutions < 1)
    {
        goto KUBE_CLEAN;
    }

    char* scr = arguments.args[0];

    init_env();

    cube_tables_generate();
    cube_tables_load();

    cube_t cube = cube_create_new_cube();
    if (cube_move_apply_move_string(&cube, scr) == 1)
    {
        goto KUBE_CLEAN;
    }

    int* solutions = malloc(sizeof(int) * 20 * number_of_solutions);
    if (cube_solvers_solve_cube(cube, solutions, number_of_solutions, arguments.verbose))
    {
        cube_print_solutions(solutions, number_of_solutions, arguments.verbose);
    }
    else
    {
        printf("Could not solve the cube :(\n");
    }
    free(solutions);

KUBE_CLEAN:
    cube_tables_free();

    return 0;
}
