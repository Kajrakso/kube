#include "_index.h"

/* 0, ..., 70 - 1 = comb(8, 4) - 1 */
uint64_t
cube_to_ccu_index(cube_t* cube){
    return ccu_combinatorials_lookup[
          extract_corner_perm(cube->corners[UBL]) * 1
        + extract_corner_perm(cube->corners[UBR]) * 8
        + extract_corner_perm(cube->corners[UFR]) * 8*8
        + extract_corner_perm(cube->corners[UFL]) * 8*8*8
    ];
}


/* 0, .., 495 - 1 = comb(12, 4) - 1 */
uint64_t
cube_to_ece_index(cube_t* cube){
    return ece_combinatorials_lookup[
          extract_edge_perm(cube->edges[FR]) * 1
        + extract_edge_perm(cube->edges[FL]) * 12
        + extract_edge_perm(cube->edges[BL]) * 12*12
        + extract_edge_perm(cube->edges[BR]) * 12*12*12
    ];
}

/* 0, ..., 153090 - 1 */
uint64_t
cube_to_c_index(cube_t* cube){
  return ccu_coud_to_c_index(
    cube_to_ccu_index(cube),
    cube_to_coud_index(cube)
  );
}

/* 0, ..., 495*2**11 - 1 = 1013760 - 1 */
uint64_t
cube_to_e_index(cube_t* cube){
  return ece_eofb_to_e_index(
    cube_to_ece_index(cube),
    cube_to_eofb_index(cube)
  );
}

/* 0, ..., 2**11 - 1 */
uint64_t
cube_to_eofb_index(cube_t* cube){
    uint64_t orien = 0;
    unsigned int pow = 1;
    for (int i = 0; i < NEDGES - 1; i++){
        orien += pow * extract_edge_orien(cube->edges[which_edge_at_pos(i, cube)], FB);
        pow *= 2;
    } 
    return orien;
}

/* 0, ..., 3**7 - 1 */
uint64_t
cube_to_coud_index(cube_t* cube){
    uint64_t orien = 0;
    unsigned int pow = 1;
    for (int i = 0; i < NCORNERS - 1; i++){
        orien += pow * extract_corner_orien(cube->corners[which_corner_at_pos(i, cube)], UD);
        pow *= 3;
    } 
    return orien;
}


inline uint64_t
ccu_coud_to_c_index(uint64_t ccu, uint64_t coud){
  return ccu + NCCU*(coud);
}

inline uint64_t
ece_eofb_to_e_index(uint64_t ece, uint64_t eofb){
  return ece + NECE*(eofb);
}


uint64_t
cclass_i_e_to_H_index(uint64_t cclass_i, uint64_t e_i){
  return cclass_i + NCCLASS * e_i;
}


uint64_t
cube_to_H_index(cube_t* cube, struct c_index_cclass_sym* cclass){
  struct c_index_cclass_sym c = cclass[cube_to_c_index(cube)];
  cube_t c2 = cube_operation_sym_conjugate(*cube, c.sym);
  uint64_t e_i = (uint64_t)cube_to_e_index(&c2);

  return cclass_i_e_to_H_index(c.cclass_i, e_i);
}

//todo: remove?
cube_t
ccu_index_to_cube(uint64_t ccu_i){
  cube_t cube = cube_create_new_cube();

  // set ccu
  // bad code. need to fix
  int m = -1;
  for (int k = 0; k < 4096; k++){
    if (ccu_combinatorials_lookup[k] == ccu_i) {
      m = k;
      break;
    }
  }
  if ( m == -1 ) {
    // OH NO!
    exit(1);
  }
  int UBL_i = m % 8;
  int UBR_i = (m % (8 * 8)) / 8;
  int UFR_i = (m % (8 * 8 * 8)) / (8 * 8);
  int UFL_i = (m / (8 * 8 * 8));

  // todo: ugly, but it works
  int e = which_corner_at_pos(UBL_i, &cube);
  int ep = extract_corner_perm(cube.corners[UBL]);
  update_corner_perm(&cube.corners[UBL], UBL_i);
  update_corner_perm(&cube.corners[e], ep);

  e = which_corner_at_pos(UBR_i, &cube);
  ep = extract_corner_perm(cube.corners[UBR]);
  update_corner_perm(&cube.corners[UBR], UBR_i);
  update_corner_perm(&cube.corners[e], ep);

  e = which_corner_at_pos(UFR_i, &cube);
  ep = extract_corner_perm(cube.corners[UFR]);
  update_corner_perm(&cube.corners[UFR], UFR_i);
  update_corner_perm(&cube.corners[e], ep);

  e = which_corner_at_pos(UFL_i, &cube);
  ep = extract_corner_perm(cube.corners[UFL]);
  update_corner_perm(&cube.corners[UFL], UFL_i);
  update_corner_perm(&cube.corners[e], ep);

  fix_eo_lr_ud(&cube);
  return cube;
}

cube_t coud_index_to_cube(uint64_t coud_i){
  cube_t cube = cube_create_new_cube();

  int c_i, co, co_sum = 0, pow = 1;

  for (int i = 0; i < NCORNERS - 1; i++){
    co = (coud_i / pow) % 3;
    c_i = which_corner_at_pos(i, &cube);

    update_corner_orien(&cube.corners[c_i], co, 0, 0);
    co_sum = co_sum + co;

    pow *= 3;
  }

  c_i = which_corner_at_pos(NCORNERS - 1, &cube);
  update_corner_orien(&cube.corners[c_i], (3 - co_sum % 3) % 3, 0, 0);

  fix_co_lr_ud(&cube);
  return cube;
}

cube_t ece_index_to_cube(uint64_t ece_i){
  cube_t cube = cube_create_new_cube();

  int m = -1;
  for (int k = 0; k < 20736; k++){
    if (ece_combinatorials_lookup[k] == ece_i) {
      m = k;
      break;
    }
  }
  if ( m == -1 ) {
    // OH NO!
    exit(1);
  }
  int BL_i = m % 12;
  int BR_i = (m % (12 * 12)) / 12;
  int FR_i = (m % (12 * 12 * 12)) / (12 * 12);
  int FL_i = (m / (12 * 12 * 12));

  // todo: ugly, but it works
  int e = which_edge_at_pos(BL_i, &cube);
  int ep = extract_edge_perm(cube.edges[BL]);
  update_edge_perm(&cube.edges[BL], BL_i);
  update_edge_perm(&cube.edges[e], ep);

  e = which_edge_at_pos(BR_i, &cube);
  ep = extract_edge_perm(cube.edges[BR]);
  update_edge_perm(&cube.edges[BR], BR_i);
  update_edge_perm(&cube.edges[e], ep);

  e = which_edge_at_pos(FR_i, &cube);
  ep = extract_edge_perm(cube.edges[FR]);
  update_edge_perm(&cube.edges[FR], FR_i);
  update_edge_perm(&cube.edges[e], ep);

  e = which_edge_at_pos(FL_i, &cube);
  ep = extract_edge_perm(cube.edges[FL]);
  update_edge_perm(&cube.edges[FL], FL_i);
  update_edge_perm(&cube.edges[e], ep);
  
  return cube;
}

// todo: remove?
cube_t eofb_index_to_cube(uint64_t eofb_i){
  cube_t cube = cube_create_new_cube();

  // set eo
  int e_i, eo, eo_sum = 0;

  for (int i = 0; i < NEDGES - 1; i++){
    eo = (eofb_i >> i) % 2;

    e_i = which_edge_at_pos(i, &cube);
    update_edge_orien(&cube.edges[e_i], eo, 0, 0);
    eo_sum ^= eo;
  }
  e_i = which_edge_at_pos(NEDGES - 1, &cube);
  update_edge_orien(&cube.edges[e_i], eo_sum, 0, 0);

  // fix eo and co
  fix_eo_lr_ud(&cube);

  return cube;
}

// need testing. well, all of these functions need testing.
cube_t
c_index_to_cube(uint64_t c_i){
  cube_t cube1 = ccu_index_to_cube(c_i % NCCU); 
  cube_t cube2 = coud_index_to_cube(c_i / NCCU);
  
  return cube_operation_compose(cube1, cube2);
}

// need testing. well, all of these functions need testing.
cube_t
e_index_to_cube(uint64_t e_i){
  cube_t cube1 = ece_index_to_cube(e_i % NECE); 
  cube_t cube2 = eofb_index_to_cube(e_i / NECE);
  
  return cube_operation_compose(cube1, cube2);
}

// TODO: rewrite/refactor/delete!
cube_t
H_index_to_cube(uint64_t H_index, struct c_index_cclass_sym* cclass){

  fprintf(stderr, "Preparing a cclass -> cindex_rep table. TODO: look into this!\n");
  // we fill a cclass -> cindex_rep table here. TODO: reconsider this.
  uint64_t cclass_index_cindex_rep[NCCLASS];
  for (uint64_t k = 0; k < NCCLASS; k++){
    for (uint64_t m = 0; m < NCCU * NCO; m++){
      if (cclass[m].cclass_i == k){
        cclass_index_cindex_rep[k] = cclass[m].cclass; 
        break;
      }
    }
  }

  uint64_t cclass_i = H_index % NCCLASS;

  uint64_t cindex_rep = cclass_index_cindex_rep[cclass_i];
  uint64_t ccu = cindex_rep % NCCU;
  uint64_t coud = cindex_rep / NCCU;

  uint64_t ei = H_index / NCCLASS;
  uint64_t ece = ei % NECE;
  uint64_t eofb = ei / NECE;

  cube_t c1 = e_index_to_cube(ei);
  cube_t c2 = c_index_to_cube(cindex_rep);

  cube_t c = cube_operation_compose(c1,c2);

  return c;
}

/* gen */

/* temp! */

static int UDsyms[] = {
  0, 1, 2, 3, 16, 17, 18, 19,     // no inversion
  24, 25, 26, 27, 40, 41, 42, 43  // inversion
};

// todo: ugly. need to rewrite
void
gen_c_sym_index_tables(){
  cube_t c1, c2;
  int sym, t;
  uint64_t best, idx;

  struct c_index_cclass_sym* cclass_table
    = malloc(sizeof(struct c_index_cclass_sym) * NCCU * NCO);

  fprintf(stderr, "initializing cclass_table with default values\n");
  for (uint64_t c_index = 0; c_index < NCCU * NCO; c_index++){
    cclass_table[c_index] = (struct c_index_cclass_sym){-1, -1, -1};
  }

  fprintf(stderr, "start to fill cclass_table with actual values\n");
  int cclass_num = -1;
  for (uint64_t c_index = 0; c_index < NCCU * NCO; c_index++){
    c1 = c_index_to_cube(c_index);
    sym = 0;
    best = c_index;
    for (int i = 1; i < 16; i++){
      t = UDsyms[i];
      c2 = cube_operation_sym_conjugate(c1, t);
      idx = cube_to_c_index(&c2);   //  todo: look into type
      if (idx < best){
        best = idx;
        sym = t;
      }
    }
    if (cclass_table[best].cclass == -1){
      cclass_num++; 

      cclass_table[c_index] = (struct c_index_cclass_sym){
        .cclass_i = cclass_num,
        .cclass = best,
        .sym = sym
      };
    } else {
      cclass_table[c_index] = (struct c_index_cclass_sym){
        .cclass_i = cclass_table[best].cclass_i,
        .cclass = best,
        .sym = sym
      };
    }
  }

  save_cclasstable(
    "cclass.dat",
    cclass_table,
    sizeof(struct c_index_cclass_sym) * NCCU * NCO
  );

  free(cclass_table);
}

// temp!

bool load_cclasstable(char* filename, struct c_index_cclass_sym* cclass_table, size_t table_size){

  FILE *file = fopen(filename, "rb");
  if (!file){
    fprintf(stderr, "Could not open file\n");
    return false;
  };
  
  fprintf(stderr, "loading cclass_table from %s\n", filename);
  if (fread(cclass_table, table_size, 1, file) != 1) {
    fprintf(stderr, "Could not read from file %s\n", filename);
    fclose(file);
    return false;
  }
  
  fclose(file);
  return true;
}

bool save_cclasstable(char* filename, struct c_index_cclass_sym * cclass_table, size_t table_size){

  fprintf(stderr, "writing cclass_table to disk. file: %s.\n", filename);

  FILE *file = fopen(filename, "wb");
  if (!file) {
    fprintf(stderr, "Was not able to open file %s\n", filename);
    return false;
  }

  // Save the array
  if (fwrite(cclass_table, table_size, 1, file) != 1){
    fclose(file);
    return false;
  }

  fclose(file);
  return true;
}

