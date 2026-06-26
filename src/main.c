#include <stdlib.h>

#include "cli.h"
#include "env.h"

const char* argp_program_version     = "kube 0.1";
const char* argp_program_bug_address = "<oskarfj@oskarfj.no>";

/* Program documentation. */
static char doc[] = "kube -- an optimal Rubik's cube solver";

/* A description of the arguments we accept. */
static char args_doc[] = "";

/* The options we understand. */
static struct argp_option options[] = {
  {"verbose", 'v', 0, 0, "Produce verbose output", 0},
  {"num", 'n', "NUM", 0,
   "Try to find NUM solutions. When multiple steps are given, kube does a beam search to find NUM solutions.",
   0},
  {"max-depth", 'M', "MAX", 0, "limit the search depth", 0},
  {"format", 'f', "FORMAT", 0, "Specify scramble format", 0},
  {"gen", 'g', 0, 0, "Generate tables", 0},
  {"step", 's', "STEP", 0,
   "Append a solving step (ordered). Can be repeated.\n"
   "Examples:\n"
   "  -s eo -s dr -s fin",
   0},
  {0}};


/* Our argp parser. */
static struct argp argp = {options, parse_opt, args_doc, doc, 0, 0, 0};


int main(int argc, char** argv) {
    struct arguments arguments;
    set_default_values_arguments(&arguments);
    argp_parse(&argp, argc, argv, 0, 0, &arguments);
    init_env();

    if (arguments.gen == 1)
    {
        cli_gen();
        return 0;
    }

    if (arguments.step_count == 0)
    {
        printf("Please provide a step to solve!\n");
        return 1;
    }

    if (arguments.number_of_solutions >= 1)
    {
        solving_step** steps = malloc(arguments.step_count * sizeof(solving_step*));
        if (steps == NULL) {
            fprintf(stderr, "Could not allocate space for steps\n");
            return 1;
        }
        
        cli_solver_prepare(arguments, steps);
        cli_solver_solving_loop(arguments, steps);
        cli_solver_cleanup(arguments, steps);

        free(steps);
    }

    return 0;
}
