#include "_tables.h"

void cube_tables_generate(){
    // gen moves
    initialize_move_tables();
    gen_move_tables();

    // gen sym
    initialize_sym_tables();
    gen_sym_tables();
}

bool cube_tables_save(const char *filename, uint16_t* table, size_t table_size) {
    FILE *file = fopen(filename, "wb");
    if (!file) return false;

    // Save the array
    if (fwrite(table, table_size, 1, file) != 1) {
        fclose(file);
        return false;
    }

    fclose(file);
    return true;
}

bool cube_tables_load(const char *filename, uint16_t* table, size_t table_size) {
    FILE *file = fopen(filename, "rb");
    if (!file) return false;

    // Load the move_table_corner_transformation array
    if (fread(table, table_size, 1, file) != 1) {
        fclose(file);
        return false;
    }

    fclose(file);
    return true;
}

// TEMP: load pruning table
bool load_ptable(char* filename, uint16_t* ptable){
  FILE *file = fopen(filename, "rb");
  if (!file){
    fprintf(stderr, "Could not open file\n");
    free(ptable);
    return false;
  };
  
  fprintf(stderr, "loading ptable from %s\n", filename);
  if (fread(ptable, sizeof(uint16_t)*SIZEL, 1, file) != 1) {
    fprintf(stderr, "Could not read from file %s\n", filename);
    fclose(file);
    free(ptable);
    return false;
  }
  
  fclose(file);
  return true;
}
