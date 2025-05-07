#include "_tables.h"

#include <stdio.h>
#include <stdlib.h>

// The table pointers is only visible in this file.
static void* ptable_H = NULL;
static void* cclass_table = NULL;

void* load_table_from_file(const char* path, const size_t table_size) {
    FILE* fp = fopen(path, "rb");
    if (!fp) return NULL;
    void* buffer = malloc(table_size);
    if (!buffer) {
        fclose(fp);
        return NULL;
    }
    fread(buffer, 1, table_size, fp);
    fclose(fp);
    return buffer;
}

int init_ptable_H(const char* path) {
  size_t ptable_H_size = sizeof(uint8_t) * SIZE_PTABLE_H;
    if (ptable_H == NULL) {
        ptable_H = load_table_from_file(path, ptable_H_size);
        if (!ptable_H) return -1;
    }
    return 0;
}

int init_cclass_table(const char* path) {
  size_t cclass_table_size = sizeof(struct c_index_cclass_sym) * NCCU * NCO;
    if (cclass_table == NULL) {
        cclass_table = load_table_from_file(path, cclass_table_size);
        if (!cclass_table) return -1;
    }
    return 0;
}

void* get_ptable_H() {
    return ptable_H;
}

void* get_cclass_table() {
    return cclass_table;
}

void free_ptable_H() {
    free(ptable_H);
    ptable_H = NULL;
}

void free_cclass_table() {
    free(cclass_table);
    cclass_table = NULL;
}

void cube_tables_generate(){
    // gen moves
    initialize_move_tables();
    gen_move_tables();

    // gen sym
    initialize_sym_tables();
    gen_sym_tables();

    // gen move mask
    gen_move_mask();

    // temp: combinatorials
    precompute_combinatorials();
}


int cube_tables_load(){
  if (init_ptable_H("data/H.dat") != 0) {
    fprintf(stderr, "Failed to load pruning table H.\n");
    return 1;
  }
  if (init_cclass_table("data/cclass.dat") != 0) {
    fprintf(stderr, "Failed to load cclass table.\n");
    return 1;
  }
  return 0;
}


void cube_tables_free(){
  free_ptable_H();
  free_cclass_table();
}

// TEMP: load pruning table
bool load_ptable(char* filename, uint8_t* ptable, size_t table_size){
  FILE *file = fopen(filename, "rb");
  if (!file){
    fprintf(stderr, "Could not open file\n");
    return false;
  };
  
  fprintf(stderr, "loading ptable from %s\n", filename);
  if (fread(ptable, table_size, 1, file) != 1) {
    fprintf(stderr, "Could not read from file %s\n", filename);
    fclose(file);
    return false;
  }
  
  fclose(file);
  return true;
}


bool save_ptable(char* filename, uint8_t* ptable, size_t table_size){
  fprintf(stderr, "writing ptable to disk. file: %s.\n", filename);

  FILE *file = fopen(filename, "wb");
  if (!file) {
    fprintf(stderr, "Was not able to open file %s\n", filename);
    return false;
  }

  // Save the array
  if (fwrite(ptable, table_size, 1, file) != 1){
    fclose(file);
    return false;
  }

  fclose(file);
  return true;
}
