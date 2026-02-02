#include "cli.h"

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

int* parse_move_string(size_t* out_length, const char* move_string) {
    // Mapping from Singmaster notation to enum move values
    const struct {
        const char* notation;
        enum move   value;
    } move_map[] = {
      {"U", U1}, {"U2", U2}, {"U'", U3}, {"D", D1}, {"D2", D2}, {"D'", D3},
      {"L", L1}, {"L2", L2}, {"L'", L3}, {"R", R1}, {"R2", R2}, {"R'", R3},
      {"F", F1}, {"F2", F2}, {"F'", F3}, {"B", B1}, {"B2", B2}, {"B'", B3},
    };

    const size_t map_size = sizeof(move_map) / sizeof(move_map[0]);

    // Split the input string into tokens
    size_t len        = strlen(move_string) + 1;  // +1 for the null terminator
    char*  input_copy = malloc(len);
    if (input_copy)
    {
        memcpy(input_copy, move_string, len);
    }
    else
    {
        perror("Failed to allocate memory");
        return NULL;
    }

    char* token = strtok(input_copy, " ");

    // Allocate an initial array to store results (resize if needed)
    size_t capacity = 16;  // Initial capacity
    size_t length   = 0;   // Actual number of moves
    int*   result   = malloc(capacity * sizeof(int));
    if (!result)
    {
        perror("Failed to allocate memory");
        free(input_copy);
        return NULL;
    }

    // Process each token
    while (token != NULL)
    {
        // Find the corresponding enum value
        int found = 0;
        for (size_t i = 0; i < map_size; i++)
        {
            if (strcmp(token, move_map[i].notation) == 0)
            {
                // Add to result array
                if (length >= capacity)
                {
                    // Resize the array
                    capacity *= 2;
                    result = realloc(result, capacity * sizeof(int));
                    if (!result)
                    {
                        perror("Failed to reallocate memory");
                        free(input_copy);
                        return NULL;
                    }
                }
                result[length++] = move_map[i].value;
                found            = 1;
                break;
            }
        }

        // Handle invalid move
        if (!found)
        {
            fprintf(stderr, "Invalid move: %s\n", token);
            free(result);
            return NULL;
        }

        // Get the next token
        token = strtok(NULL, " ");
    }

    // Clean up and set output length
    free(input_copy);
    *out_length = length;
    return result;
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
        if (verbose == 1)
        {
            printf("(%i)\n", len);
        }
        else
        {
            printf("\n");
        }
    }
}


void cube_print_solution_set(SolutionSet* solution_set, int verbose){
    int count = solution_set->count;
    Solution* solutions = solution_set->data;
    for (int solution_idx = 0; solution_idx < count; solution_idx++){
        Solution solution = solutions[solution_idx]; 
        for (int move_idx = 0; move_idx < solution.length; move_idx++){
            int move = solution.moves[move_idx];
            if (is_valid_move(move)) {
                printf("%s ", move_notation[move]);
            }
            else {
                printf("? ");
            }
        }
        if (verbose == 1) {
            printf("(%i)\n", solution.length);
        }
        else {
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


void set_default_values_arguments(struct arguments* arguments){
    /* Default values. */
    arguments->verbose             = 0;
    arguments->gen                 = 0;
    arguments->format              = "singmaster";
    // arguments->steps[0]            = (struct step){.name = "fin", .max_depth = -1};
    arguments->step_count          = 0;
    arguments->number_of_solutions = 1;
}
