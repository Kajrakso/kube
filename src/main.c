#include <stdlib.h>
#include <time.h>

#include "solutions.h"
#include "cli.h"
#include "scrambler.h"
#include "env.h"

#include "tables_ptable_data.h"
#include "solver_steps.h"
#include "solver_pipeline.h"

#define BUF_SIZE 4096

const char* argp_program_version     = "kube 0.1";
const char* argp_program_bug_address = "<oskarfj@oskarfj.no>";

/* Program documentation. */
static char doc[] = "kube -- an optimal Rubik's cube solver";

/* A description of the arguments we accept. */
static char args_doc[] = "";

/* The options we understand. */
static struct argp_option options[] = {{"verbose", 'v', 0, 0, "Produce verbose output", 0},
                                       {"num", 'n', "NUM", 0, "Try to find NUM solutions. When multiple steps are given, kube does a beam search to find NUM solutions.", 0},
                                       {"format", 'f', "FORMAT", 0, "Specify scramble format", 0},
                                       {"gen", 'g', 0, 0, "Generate tables", 0},
                                       {"step", 's', "STEP", 0,
                                        "Append a solving step (ordered). Can be repeated.\n"
                                        "Examples:\n"
                                        "  -s eo -s dr -s fin", 0},
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
        printf("Starting to gen tables...\n");
        char        fname[strlen(tabledir) + FILENAME_MAX];

        // this is needed since kube currently
        // does not generate this file itself
        strcpy(fname, tabledir);
        strcat(fname, "/");
        printf("TEMP: If you want to solve to HTR you need to copy dr_subsets.dat to this location: %s\n", fname);

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
        return 0;
    }

    if (arguments.step_count == 0){
        printf("Please provide a step to solve!\n");
        return 1;
    }

    // if (arguments.number_of_solutions > 1 && arguments.step_count > 1){
    //     printf("-n is not supported with multiple steps. Uses n = 1.");
    //     arguments.number_of_solutions = 1;
    // }

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

            if (ss == NULL){
                printf("Did not understand step. exiting...\n");
                return 1;
            }

            if (ss->p_data == NULL){
                if (arguments.verbose == 1)
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

            if (ss->solving_type == SOLVE_HTR){
                cube_tables_load_dr_subsets();
            }

            steps[i] = ss;
        }

        char* buf       = malloc(BUF_SIZE);
        while (fgets(buf, BUF_SIZE, stdin))
        {
            buf[strcspn(buf, "\r\n")] = 0;

            cube_t c = cube_create_new_cube();
            cube_scrambler_scramble_cube(&c, buf, arguments.format);

            if (arguments.step_count == 1 || arguments.number_of_solutions == 1) {
                // we invoke a simple pipeline solver:
                solver_pipeline(c, arguments, steps);
            }
            else {
                // we invoke a beam search since we have multiple steps and multiple solutions
                solver_beam_search(c, arguments, steps);
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
        free(buf);
    }

    return 0;
}
