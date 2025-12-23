#include "tables.h"
#include "index.h"
#include "tables_ptable_data.h"

#include <time.h>


// I really should try to make
// genning a ptable is just one call
// to the same function somehow

void DLS_opt1(uint64_t  ece,
           uint64_t  eofb,
           uint64_t  coud,
           uint64_t  ccu,
           uint64_t  p,
           int       remaining_moves,
           int       prev_move,
           int       num_moves_done,
           ptable_data_t ptable_data,
           uint8_t*  ptable,            // we are creating the ptable, so do not use the ptable pointer in ptable_data!
           uint64_t* sym_table_e_index) {
    if (remaining_moves == 0)
        return;
    uint64_t p2, ccu2, coud2, ece2, eofb2, e2, e2_sym;
    uint32_t mm = move_mask[prev_move];

    for (int m = 0; m < NMOVES; m++)
    {
        if (!(mm & (1 << m)))
            continue;

        ccu2  = move_table_ccu_index[ccu][m];
        coud2 = move_table_coud_index[coud][m];
        ece2  = move_table_ece_index[ece][m];
        eofb2 = move_table_eofb_index[eofb][m];

        struct c_index_cclass_sym c = cclass_table[cc_co_to_c_index(ccu2, coud2)];

        e2     = ec_eo_to_e_index(ece2, eofb2);
        e2_sym = sym_table_e_index[e2 * NSYMS + c.sym];

        p2 = cclass_i_e_to_H_index(c.cclass_i, e2_sym);

        if (p2 == p)
            continue;

        if (ptable_data.read_value_ptable_func(p2, ptable) > num_moves_done + 1)
        {
            ptable_data.set_value_ptable_func(p2, (uint8_t) (num_moves_done + 1), ptable);
        }

        DLS_opt1(ece2, eofb2, coud2, ccu2, p2, remaining_moves - 1, m, num_moves_done + 1, ptable_data, ptable,
              sym_table_e_index);
    }
}

void gen_ptable_opt1() {
    ptable_data_t ptable_data = ptable_data_opt1;

    cube_tables_load_sym_table_e_index();
    uint64_t* sym_table_e_index = (uint64_t*) get_sym_table_e_index();
    if (!sym_table_e_index)
    {
        fprintf(stderr, "Could not load sym_table_e_index. Have you initialized it?\n");
        return;
    }
    cube_t cube = cube_create_new_cube();

    gen_move_table_ece_index();
    gen_move_table_coud_index();
    gen_move_table_eofb_index();
    gen_move_table_ccu_index();
    gen_c_sym_index_tables();

    fprintf(stderr, "Initializing ptable for group H\n");
    uint8_t* ptable_H = malloc(ptable_data.ptable_size);
    if (ptable_H == NULL)
    {
        fprintf(stderr, "Could not allocate memory for ptable.\n");
        return;
    }

    time_t start, end;

    start = clock();

    // calculate the initial index
    uint64_t ccu  = cube_to_cc_index(&cube, UD);
    uint64_t coud = cube_to_co_index(&cube, UD);
    uint64_t ece  = cube_to_ec_index(&cube, UD);
    uint64_t eofb = cube_to_eo_index(&cube, UD);
    uint64_t p    = cube_to_H_index(&cube, UD);

    fprintf(stderr, "Setting default values\n");
    for (uint64_t i = 0; i < ptable_data.number_of_elements; i++)
    {
        ptable_data.set_value_ptable_func(i, 15, ptable_H);
    }

    ptable_data.set_value_ptable_func(p, 0, ptable_H);

    /* TODO: improve table gen!
   * https://github.com/sebastianotronto/h48/blob/master/doc/h48.md#4-bits-tables-for-h0-and-h11
   */
    for (int depth = 0; depth < 10; depth++)
    {
        fprintf(stderr, "Searching at depth %i\n", depth);
        DLS_opt1(ece, eofb, coud, ccu, p, depth, 18, 0, ptable_data, ptable_H, sym_table_e_index);
    }

    // we fill a cclass -> cindex_rep table here. TODO: reconsider this.
    fprintf(stderr, "Preparing a cclass -> cindex_rep table. TODO: look into this!\n");
    uint64_t cclass_index_cindex_rep[NCCLASS];
    for (uint64_t k = 0; k < NCCLASS; k++)
    {
        for (uint64_t m = 0; m < NCCU * NCO; m++)
        {
            if (cclass_table[m].cclass_i == k)
            {
                cclass_index_cindex_rep[k] = cclass_table[m].cclass;
                break;
            }
        }
    }

    fprintf(stderr, "Search from nbhs. TODO: For whcih depth should we start doing this?\n");
    for (uint8_t k = 10; k < 13; k++)
    {
        fprintf(stderr, "Searching at depth %i\n", k);
        for (uint64_t p = 0; p < ptable_data.number_of_elements; p++)
        {
            if (p % 1000000000UL == 0)
            {
                fprintf(stderr, "p = %lu\n", p);
            }
            if (ptable_data.read_value_ptable_func(p, ptable_H) == 15)
            {
                uint64_t cclass_i = p % NCCLASS;
                uint64_t ei       = p / NCCLASS;

                uint64_t cindex_rep = cclass_index_cindex_rep[cclass_i];
                uint64_t ccu        = cindex_rep % NCCU;
                uint64_t coud       = cindex_rep / NCCU;

                uint64_t ece  = ei % NECE;
                uint64_t eofb = ei / NECE;

                // we check all the nbhs.
                for (int m = 0; m < NMOVES; m++)
                {
                    uint64_t ccu2  = move_table_ccu_index[ccu][m];
                    uint64_t coud2 = move_table_coud_index[coud][m];
                    uint64_t ece2  = move_table_ece_index[ece][m];
                    uint64_t eofb2 = move_table_eofb_index[eofb][m];

                    struct c_index_cclass_sym c = cclass_table[cc_co_to_c_index(ccu2, coud2)];

                    uint64_t e2 = ec_eo_to_e_index(ece2, eofb2);

                    // we need to apply symmetry conj. to the edge index as well.
                    uint64_t e2_sym = sym_table_e_index[e2 * NSYMS + c.sym];
                    uint64_t p2     = cclass_i_e_to_H_index(c.cclass_i, e2_sym);

                    if (ptable_data.read_value_ptable_func(p2, ptable_H) == k - 1)
                    {
                        ptable_data.set_value_ptable_func(p, k, ptable_H);
                        break;
                    }
                    else if (k == 12)
                    {
                        // set the remaining cosets to 13,
                        // if they are actually a 12
                        // the break; above makes sure that
                        // it is not set to 13.
                        ptable_data.set_value_ptable_func(p, 13, ptable_H);
                    }
                }
            }
        }
    }

    end = clock();

    printf("time for gen function: %f s\n", (float) (end - start) / CLOCKS_PER_SEC);

    /* end of TODO */
    char fname[strlen(tabledir) + FILENAME_MAX];

    strcpy(fname, tabledir);
    strcat(fname, "/");
    strcat(fname, ptable_data.filename);

    save_table_to_file(fname, ptable_H, ptable_data.ptable_size);

    free(ptable_H);
}


void analyze_ptable(ptable_data_t ptable_data){
    if (!ptable_data.ptable_is_loaded){
        fprintf(stderr, "UPPSI, we aint got tabl\n");
        return;
    }

    uint8_t* ptable = (uint8_t*) ptable_data.ptable;

    if (!ptable)
    {
        fprintf(stderr, "UPPSI, we aint got tabl 2\n");
        return;
    }

    uint64_t stats[21] = {0};

    uint64_t other = 0;
    for (uint64_t i = 0; i < ptable_data.number_of_elements; i++)
    {
        uint8_t val = ptable_data.read_value_ptable_func(i, ptable);
        // if (val == 15)
        // {
        //     other++;
        // }
        // else
        // {
            stats[val]++;
        // }
    }

    for (int i = 0; i <= 20; i++)
    {
        printf("%i: %lu\n", i, stats[i]);
    }
    printf("other: %lu\n", other);
}









void DLS_DR(uint64_t  eofb,
           uint64_t  ece,
           uint64_t  coud,
           uint64_t  p,
           int       remaining_moves,
           int       prev_move,
           int       num_moves_done,
           ptable_data_t ptable_data,
           uint8_t*  ptable            // we are creating the ptable, so do not use the ptable pointer in ptable_data!
           ) {
    if (remaining_moves == 0)
        return;
    uint64_t p2, coud2, ece2, eofb2, e2;
    uint32_t mm = move_mask[prev_move];

    for (int m = 0; m < NMOVES; m++)
    {
        if (!(mm & (1 << m)))
            continue;

        coud2 = move_table_coud_index[coud][m];
        eofb2 = move_table_eofb_index[eofb][m];
        ece2 = move_table_ece_index[ece][m];

        p2 = coud2 + NCO * (eofb2 + NEO * ece2);


        if (p2 == p)
            continue;

        if (ptable_data.read_value_ptable_func(p2, ptable) > num_moves_done + 1)
        {
            ptable_data.set_value_ptable_func(p2, (uint8_t) (num_moves_done + 1), ptable);
        }

        DLS_DR(eofb2, ece2, coud2, p2, remaining_moves - 1, m, num_moves_done + 1, ptable_data, ptable);
    }
}


void gen_ptable_DR() {
    ptable_data_t ptable_data = ptable_data_dr;

    cube_t cube = cube_create_new_cube();

    gen_move_table_coud_index();
    gen_move_table_eofb_index();
    gen_move_table_ece_index();

    fprintf(stderr, "Initializing ptable for %s\n", ptable_data.name);
    uint8_t* ptable = malloc(ptable_data.ptable_size);
    if (ptable == NULL)
    {
        fprintf(stderr, "Could not allocate memory for ptable %s.\n", ptable_data.name);
        return;
    }

    time_t start, end;

    start = clock();

    // calculate the initial index
    uint64_t coud = cube_to_co_index(&cube, UD);
    uint64_t eofb = cube_to_eo_index(&cube, UD);
    uint64_t eceud = cube_to_ec_index(&cube, UD);
    uint64_t p    = cube_to_DR_index(&cube, UD);

    fprintf(stderr, "Setting default values\n");
    for (uint64_t i = 0; i < ptable_data.number_of_elements; i++)
    {
        ptable_data.set_value_ptable_func(i, 15, ptable);
    }

    ptable_data.set_value_ptable_func(p, 0, ptable);

    for (int depth = 0; depth < 9; depth++)
    {
        fprintf(stderr, "Searching at depth %i\n", depth);
        DLS_DR(eofb, eceud, coud, p, depth, 18, 0, ptable_data, ptable);
    }


    fprintf(stderr, "Search from nbhs. TODO: For whcih depth should we start doing this?\n");
    for (uint8_t k = 9; k < 12; k++)
    {
        fprintf(stderr, "Searching at depth %i\n", k);
        for (uint64_t p = 0; p < ptable_data.number_of_elements; p++)
        {
            if (p % 1000000000UL == 0)
            {
                fprintf(stderr, "p = %lu\n", p);
            }
            if (ptable_data.read_value_ptable_func(p, ptable) == 15)
            {
                uint64_t coud = p % NCO;
                uint64_t ece  = (p / NCO) / NEO;
                uint64_t eofb = (p / NCO) % NEO;

                // we check all the nbhs.
                for (int m = 0; m < NMOVES; m++)
                {
                    uint64_t coud2 = move_table_coud_index[coud][m];
                    uint64_t ece2  = move_table_ece_index[ece][m];
                    uint64_t eofb2 = move_table_eofb_index[eofb][m];

                    uint64_t p2 = coud2 + NCO * (eofb2 + NEO * ece2);

                    if (ptable_data.read_value_ptable_func(p2, ptable) == k - 1)
                    {
                        ptable_data.set_value_ptable_func(p, k, ptable);
                        break;
                    }
                    else if (k == 11)
                    {
                        // set the remaining cosets to 12,
                        // if they are actually a 11
                        // the break; above makes sure that
                        // it is not set to 12.
                        ptable_data.set_value_ptable_func(p, 12, ptable);
                    }
                }
            }
        }
    }

    end = clock();

    printf("time for gen function: %f s\n", (float) (end - start) / CLOCKS_PER_SEC);

    /* end of TODO */
    char fname[strlen(tabledir) + FILENAME_MAX];

    strcpy(fname, tabledir);
    strcat(fname, "/");
    strcat(fname, ptable_data.filename);

    save_table_to_file(fname, ptable, ptable_data.ptable_size);

    free(ptable);
}






// I genned a file with cp <-> dr_subset for all cp using GAP.
bool parse_cp_to_dr_subset_file_and_save_dr_subset_table(char* filename){
    int cp_subsets[FACTORIAL8];

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    int count = 0;
    while (count < FACTORIAL8) {
        int arr[NCORNERS];
        int label;

        int n = fscanf(fp,
            " [ %d , %d , %d , %d , %d , %d , %d , %d ] : %d",
            &arr[0], &arr[1], &arr[2], &arr[3],
            &arr[4], &arr[5], &arr[6], &arr[7],
            &label
        );

        if (n != 9) {
            break;  // EOF or malformed line
        }

        for (int i = 0; i < NCORNERS; i++){
            arr[i]--; // GAP uses 1..8 instead of 0..7.
        }

        count++;

        cp_subsets[perm_to_fact(arr, NCORNERS)] = label;
    }


    /* end of TODO */
    char fname[strlen(tabledir) + FILENAME_MAX];

    strcpy(fname, tabledir);
    strcat(fname, "/");
    strcat(fname, "dr_subsets.dat");

    save_table_to_file(fname, cp_subsets, FACTORIAL8 * sizeof(int));

    fclose(fp);

    return 0;


}
