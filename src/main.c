#include <stdlib.h>

#include "cli.h"
#include "env.h"

const char* argp_program_version     = "kube 0.1.0"; /* "major version"."minor version"."bugfixes" */
const char* argp_program_bug_address = "<oskarfj@oskarfj.no>";

/* Program documentation. */
static char doc[] = "kube -- an optimal Rubik's cube solver";

/* A description of the arguments we accept. */
static char args_doc[] = "[SCRAMBLE]";

/* The options we understand. */
static struct argp_option options[] = {
  {"verbose", 'v', 0, 0, "Produce verbose output.", 0},
  {"stdin", 'i', 0, 0, "Read scrambles from standard input.", 0},
  {"num", 'n', "NUM", 0,
   "Try to find NUM solutions. When multiple steps are given, kube does a beam search to find NUM solutions.",
   0},
  {"max-depth", 'M', "MAX", 0, "Limit the search depth to MAX moves.", 0},
  {"threads", 't', "NUM", 0, "Specify number of threads to use during search. defaults to number of cpus on the system.", 0},
  {"format", 'f', "FORMAT", 0, "Specify scramble format.", 0},
  {"gen", 'g', 0, 0, "Generate tables.", 0},
  {"step", 's', "STEP", 0,
   "Append a solving step (ordered). Can be repeated.\n"
   "Examples:\n"
   "  -s eo -s dr -s fin",
   0},
  {0}};


/* Our argp parser. */
static struct argp argp = {options, parse_opt, args_doc, doc, 0, 0, 0};

/* Small help info in case of fail. */
static void print_help_hint(const char* err) {
    fprintf(stderr, "%s\n\n", err);
    fprintf(stderr, "Try `kube --help' or `kube --usage' for more information.\n");
}


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
        print_help_hint("Please provide a step to solve!");
        return 1;
    }

    if (arguments.number_of_solutions >= 1)
    {
        if (arguments.step_count < 0) {
            print_help_hint("Step count is negative.");
            return 1;
        }
        solving_step** steps = malloc((size_t)arguments.step_count * sizeof(solving_step*));
        if (steps == NULL) {
            fprintf(stderr, "Could not allocate space for steps\n");
            return 1;
        }
       
        cli_solver_prepare(arguments, steps);
        if (arguments.stdin_mode == 1) {
            if (strcmp(arguments.scramble, "") != 0) {
                print_help_hint("Providing scrambles through both cli argument and stdin is not supported."); 
            }
            else {
                cli_solver_solving_loop(arguments, steps);
            }
        }
        else if (strcmp(arguments.scramble, "") != 0) {
            cli_solver_solve(arguments, steps);
        }
        else {
            print_help_hint("No scramble provided");
        }

        cli_solver_cleanup(arguments, steps);

        free(steps);
    }

    return 0;
}
