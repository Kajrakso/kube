#include "index.h"

// todo: rethink how this should be done. works for now.

uint16_t sym_table_corner_transformation[NSYMS][NCORNERCUBIES];
uint16_t sym_table_edge_transformation[NSYMS][NEDGECUBIES];

// uint64_t sym_table_e_index[NECE*NEO][NSYMS];

struct c_index_cclass_sym cclass_table[NCCU * NCO];

// todo: temp! where to place? how to handle?
/* these are the 16 symmetries that keep the UD-axis fixed in place. */
static int UDsyms[] = {
  0,  1,  2,  3,  16, 17, 18, 19,  // no inversion
  24, 25, 26, 27, 40, 41, 42, 43   // inversion
};

/* Fills the tables with default values.
This have to be done before we generate the
actual conjugations. */
void initialize_sym_tables() {
    for (int s = 0; s < NSYMS; s++)
    {
        for (uint16_t c = 0; c < NCORNERCUBIES; c++)
        {
            sym_table_corner_transformation[s][c] = c;
        }
        for (uint16_t e = 0; e < NEDGECUBIES; e++)
        {
            sym_table_edge_transformation[s][e] = e;
        }
    }
}

/*
    todo: explain how we go from integer to element of O_h
    20 for-loops. woho!
*/
void gen_sym_tables() {
    /* The idea is that we place
  each possible cubie somewhere
  on the cube and then act on
  the cube by the symmetry given
  by (x1, x2, x3, x4). */
    cube_t cube = cube_create_new_cube();

    for (int x4 = 0; x4 < 2; x4++)
    {
        for (int x3 = 0; x3 < 2; x3++)
        {
            for (int x2 = 0; x2 < 3; x2++)
            {
                for (int x1 = 0; x1 < 4; x1++)
                {
                    for (int cp = 0; cp < NCORNERS; cp++)
                    {
                        for (int co_fb = 0; co_fb < 3; co_fb++)
                        {
                            for (int co_lr = 0; co_lr < 3; co_lr++)
                            {
                                for (int co_ud = 0; co_ud < 3; co_ud++)
                                {
                                    uint16_t old_c = build_corner(cp, co_fb, co_lr, co_ud);

                                    int c_we_follow           = which_corner_at_pos(cp, &cube);
                                    cube.corners[c_we_follow] = old_c;

                                    for (int l = 0; l < x4; l++)
                                    {
                                        do_inversion(&cube);
                                    }
                                    for (int i = 0; i < x1; i++)
                                    {
                                        do_y_rot(&cube);
                                    }
                                    for (int j = 0; j < x2; j++)
                                    {
                                        do_y_rot(&cube);
                                        do_z_rot(&cube);
                                    }
                                    for (int k = 0; k < x3; k++)
                                    {
                                        do_y_rot(&cube);
                                        do_y_rot(&cube);
                                        do_z_rot(&cube);
                                        do_z_rot(&cube);
                                        do_z_rot(&cube);
                                    }

                                    uint16_t new_c = cube.corners[c_we_follow];

                                    sym_table_corner_transformation[24 * x4 + 12 * x3 + 4 * x2 + x1]
                                                                   [old_c] = new_c;
                                }
                            }
                        }
                    }
                    for (int ep = 0; ep < NEDGES; ep++)
                    {
                        for (int eo_fb = 0; eo_fb < 2; eo_fb++)
                        {
                            for (int eo_lr = 0; eo_lr < 2; eo_lr++)
                            {
                                for (int eo_ud = 0; eo_ud < 2; eo_ud++)
                                {
                                    uint16_t old_e = build_edge(ep, eo_fb, eo_lr, eo_ud);

                                    int e_we_follow         = which_edge_at_pos(ep, &cube);
                                    cube.edges[e_we_follow] = old_e;

                                    for (int l = 0; l < x4; l++)
                                    {
                                        do_inversion(&cube);
                                    }
                                    for (int i = 0; i < x1; i++)
                                    {
                                        do_y_rot(&cube);
                                    }
                                    for (int j = 0; j < x2; j++)
                                    {
                                        do_y_rot(&cube);
                                        do_z_rot(&cube);
                                    }
                                    for (int k = 0; k < x3; k++)
                                    {
                                        do_y_rot(&cube);
                                        do_y_rot(&cube);
                                        do_z_rot(&cube);
                                        do_z_rot(&cube);
                                        do_z_rot(&cube);
                                    }

                                    uint16_t new_e = cube.edges[e_we_follow];

                                    sym_table_edge_transformation[24 * x4 + 12 * x3 + 4 * x2 + x1]
                                                                 [old_e] = new_e;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void gen_sym_table_e_index() {
    size_t table_size = sizeof(uint64_t) * NECE * NEO * NSYMS;
    fprintf(stderr, "Initializing table for sym_table_e_index\n");
    uint64_t* sym_table_e_index = malloc(table_size);
    for (int c1 = 0; c1 < 9; c1++)
    {
        for (int c2 = c1 + 1; c2 < 10; c2++)
        {
            for (int c3 = c2 + 1; c3 < 11; c3++)
            {
                for (int c4 = c3 + 1; c4 < 12; c4++)
                {
                    for (int O1 = 0; O1 < 2; O1++)
                    {
                        for (int O2 = 0; O2 < 2; O2++)
                        {
                            for (int O3 = 0; O3 < 2; O3++)
                            {
                                for (int O4 = 0; O4 < 2; O4++)
                                {
                                    for (int O5 = 0; O5 < 2; O5++)
                                    {
                                        for (int O6 = 0; O6 < 2; O6++)
                                        {
                                            for (int O7 = 0; O7 < 2; O7++)
                                            {
                                                for (int O8 = 0; O8 < 2; O8++)
                                                {
                                                    for (int O9 = 0; O9 < 2; O9++)
                                                    {
                                                        for (int O10 = 0; O10 < 2; O10++)
                                                        {
                                                            for (int O11 = 0; O11 < 2; O11++)
                                                            {
                                                                for (int k = 0; k < 16; k++)
                                                                {
                                                                    int t = UDsyms[k];

                                                                    cube_t cube;
                                                                    int    e;

                                                                    cube = cube_create_new_cube();

                                                                    e =
                                                                      which_edge_at_pos(c1, &cube);
                                                                    update_edge_perm(
                                                                      &cube.edges[BL], c1);
                                                                    update_edge_perm(&cube.edges[e],
                                                                                     BL);

                                                                    e =
                                                                      which_edge_at_pos(c2, &cube);
                                                                    update_edge_perm(
                                                                      &cube.edges[BR], c2);
                                                                    update_edge_perm(&cube.edges[e],
                                                                                     BR);

                                                                    e =
                                                                      which_edge_at_pos(c3, &cube);
                                                                    update_edge_perm(
                                                                      &cube.edges[FR], c3);
                                                                    update_edge_perm(&cube.edges[e],
                                                                                     FR);

                                                                    e =
                                                                      which_edge_at_pos(c4, &cube);
                                                                    update_edge_perm(
                                                                      &cube.edges[FL], c4);
                                                                    update_edge_perm(&cube.edges[e],
                                                                                     FL);

                                                                    update_edge_orien(
                                                                      &cube.edges[which_edge_at_pos(
                                                                        UB, &cube)],
                                                                      O1, 0, 0);
                                                                    update_edge_orien(
                                                                      &cube.edges[which_edge_at_pos(
                                                                        UR, &cube)],
                                                                      O2, 0, 0);
                                                                    update_edge_orien(
                                                                      &cube.edges[which_edge_at_pos(
                                                                        UF, &cube)],
                                                                      O3, 0, 0);
                                                                    update_edge_orien(
                                                                      &cube.edges[which_edge_at_pos(
                                                                        UL, &cube)],
                                                                      O4, 0, 0);
                                                                    update_edge_orien(
                                                                      &cube.edges[which_edge_at_pos(
                                                                        DF, &cube)],
                                                                      O5, 0, 0);
                                                                    update_edge_orien(
                                                                      &cube.edges[which_edge_at_pos(
                                                                        DR, &cube)],
                                                                      O6, 0, 0);
                                                                    update_edge_orien(
                                                                      &cube.edges[which_edge_at_pos(
                                                                        DB, &cube)],
                                                                      O7, 0, 0);
                                                                    update_edge_orien(
                                                                      &cube.edges[which_edge_at_pos(
                                                                        DL, &cube)],
                                                                      O8, 0, 0);
                                                                    update_edge_orien(
                                                                      &cube.edges[which_edge_at_pos(
                                                                        BL, &cube)],
                                                                      O9, 0, 0);
                                                                    update_edge_orien(
                                                                      &cube.edges[which_edge_at_pos(
                                                                        BR, &cube)],
                                                                      O10, 0, 0);
                                                                    update_edge_orien(
                                                                      &cube.edges[which_edge_at_pos(
                                                                        FR, &cube)],
                                                                      O11, 0, 0);
                                                                    update_edge_orien(
                                                                      &cube.edges[which_edge_at_pos(
                                                                        FL, &cube)],
                                                                      (O1 + O2 + O3 + O4 + O5 + O6
                                                                       + O7 + O8 + O9 + O10 + O11)
                                                                        % 2,
                                                                      0, 0);
                                                                    fix_eo_lr_ud(&cube);
                                                                    fix_co_lr_ud(&cube);

                                                                    uint64_t ece_i =
                                                                      cube_to_ec_index(&cube, UD);
                                                                    uint64_t eofb_i =
                                                                      cube_to_eo_index(&cube, UD);
                                                                    cube =
                                                                      cube_operation_sym_conjugate(
                                                                        cube, t);
                                                                    uint64_t ece_ii =
                                                                      cube_to_ec_index(&cube, UD);
                                                                    uint64_t eofb_ii =
                                                                      cube_to_eo_index(&cube, UD);

                                                                    sym_table_e_index
                                                                      [ec_eo_to_e_index(ece_i,
                                                                                        eofb_i)
                                                                         * NSYMS
                                                                       + t] =
                                                                        ec_eo_to_e_index(ece_ii,
                                                                                         eofb_ii);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    char fname[strlen(tabledir) + 100];

    strcpy(fname, tabledir);
    strcat(fname, "/");
    strcat(fname, "sym_table_e_index.dat");

    save_table_to_file(fname, sym_table_e_index, table_size);
    free(sym_table_e_index);
}

// todo: ugly. need to rewrite
/* generates a table that maps a c-index to a pair (cclass_i, cclass, sym) */
void gen_c_sym_index_tables() {
    cube_t   c1, c2;
    int      sym, t;
    uint64_t best, idx;

    // fprintf(stderr, "initializing cclass_table with default values\n");
    for (uint64_t c_index = 0; c_index < NCCU * NCO; c_index++)
    {
        cclass_table[c_index] = (struct c_index_cclass_sym) {-1, -1, -1};
    }

    // fprintf(stderr, "start to fill cclass_table with actual values\n");
    int cclass_num = -1;
    for (uint64_t c_index = 0; c_index < NCCU * NCO; c_index++)
    {
        c1   = c_index_to_cube(c_index);
        sym  = 0;
        best = c_index;
        for (int i = 1; i < 16; i++)
        {
            t   = UDsyms[i];
            c2  = cube_operation_sym_conjugate(c1, t);
            idx = cube_to_c_index(&c2, UD);  //  todo: look into type
            if (idx < best)
            {
                best = idx;
                sym  = t;
            }
        }
        if (cclass_table[best].cclass == -1)
        {
            cclass_num++;

            cclass_table[c_index] =
              (struct c_index_cclass_sym) {.cclass_i = cclass_num, .cclass = best, .sym = sym};
        }
        else
        {
            cclass_table[c_index] = (struct c_index_cclass_sym) {
              .cclass_i = cclass_table[best].cclass_i, .cclass = best, .sym = sym};
        }
    }
}
