#include "tables.h"
#include "index.h"

void DLS_H(uint64_t  ece,
           uint64_t  eofb,
           uint64_t  coud,
           uint64_t  ccu,
           uint64_t  p,
           int       remaining_moves,
           int       prev_move,
           int       num_moves_done,
           uint8_t*  ptable,
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

        if (ptable_read_val(p2, ptable) > num_moves_done + 1)
        {
            ptable_set_val(p2, (uint8_t) (num_moves_done + 1), ptable);
        }

        DLS_H(ece2, eofb2, coud2, ccu2, p2, remaining_moves - 1, m, num_moves_done + 1, ptable,
              sym_table_e_index);
    }
}

void gen_ptable_H() {
    uint64_t* sym_table_e_index = (uint64_t*) get_sym_table_e_index();
    if (!sym_table_e_index)
    {
        fprintf(stderr, "Could not load sym_table_e_index. Have you initialized it?\n");
        return;
    }
    cube_t cube = cube_create_new_cube();

    const uint64_t size_ptable_h = SIZE_PTABLE_H;

    fprintf(stderr, "Generating move tables\n");
    fprintf(stderr, "gen_move_table_ece_index\n");
    gen_move_table_ece_index();
    fprintf(stderr, "gen_move_table_coud_index\n");
    gen_move_table_coud_index();
    fprintf(stderr, "gen_move_table_eofb_index\n");
    gen_move_table_eofb_index();
    fprintf(stderr, "gen_move_table_ccu_index\n");
    gen_move_table_ccu_index();
    fprintf(stderr, "gen_c_sym_index_tables\n");
    gen_c_sym_index_tables();

    fprintf(stderr, "Initializing ptable for group H\n");
    uint8_t* ptable_H = malloc(size_ptable_h);
    if (ptable_H == NULL)
    {
        fprintf(stderr, "Could not allocate memory for ptable.\n");
        return;
    }

    // calculate the initial index
    uint64_t ccu  = cube_to_cc_index(&cube, UD);
    uint64_t coud = cube_to_co_index(&cube, UD);
    uint64_t ece  = cube_to_ec_index(&cube, UD);
    uint64_t eofb = cube_to_eo_index(&cube, UD);

    fprintf(stderr, "Setting default values\n");
    for (uint64_t i = 0; i < SIZEH; i++)
    {
        ptable_set_val(i, 15, ptable_H);
    }

    uint64_t p = cube_to_H_index(&cube, UD);
    ptable_set_val(p, 0, ptable_H);

    /* TODO: improve table gen!
   * https://github.com/sebastianotronto/h48/blob/master/doc/h48.md#4-bits-tables-for-h0-and-h11
   */
    for (int depth = 0; depth < 10; depth++)
    {
        fprintf(stderr, "Searching at depth %i\n", depth);
        DLS_H(ece, eofb, coud, ccu, p, depth, 18, 0, ptable_H, sym_table_e_index);
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
        for (uint64_t p = 0; p < SIZEH; p++)
        {
            if (p % 1000000000UL == 0)
            {
                fprintf(stderr, "p = %lu\n", p);
            }
            if (ptable_read_val(p, ptable_H) == 15)
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

                    if (ptable_read_val(p2, ptable_H) == k - 1)
                    {
                        ptable_set_val(p, k, ptable_H);
                        break;
                    }
                    else if (k == 12)
                    {
                        // set the remaining cosets to 13,
                        // if they are actually a 12
                        // the break; above makes sure that
                        // it is not set to 13.
                        ptable_set_val(p, 13, ptable_H);
                    }
                }
            }
        }
    }

    /* end of TODO */
    char fname[strlen(tabledir) + 100];

    strcpy(fname, tabledir);
    strcat(fname, "/");
    strcat(fname, "H.dat");

    save_table_to_file(fname, ptable_H, SIZE_PTABLE_H);

    free(ptable_H);
}

void check_Hdat() {
    uint8_t* ptable = (uint8_t*) get_ptable_H();

    if (!ptable)
    {
        fprintf(stderr, "UPPSI, we aint got tabl\n");
        return;
    }

    uint64_t stats[21] = {0};

    uint64_t other = 0;
    for (uint64_t i = 0; i < SIZEH; i++)
    {
        uint8_t val = ptable_read_val(i, ptable);
        if (val == 15)
        {
            other++;
        }
        else
        {
            stats[val]++;
        }
    }

    for (int i = 0; i < 21; i++)
    {
        printf("%i: %lu\n", i, stats[i]);
    }
    printf("other: %lu\n", other);
}
