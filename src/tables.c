#include "_tables.h"

void cube_tables_generate(){
    // gen moves
    initialize_move_tables();
    gen_move_tables();

    // gen sym
    initialize_sym_tables();
    gen_sym_tables();
}

bool cube_tables_save(const char *filename, uint16_t* table) {
    FILE *file = fopen(filename, "wb");
    if (!file) return false;

    // Save the array
    if (fwrite(table, sizeof(table), 1, file) != 1) {
        fclose(file);
        return false;
    }

    fclose(file);
    return true;
}

bool cube_tables_load(const char *filename, uint16_t* table) {
    FILE *file = fopen(filename, "rb");
    if (!file) return false;

    // Load the move_table_corner_transformation array
    if (fread(table, sizeof(table), 1, file) != 1) {
        fclose(file);
        return false;
    }

    fclose(file);
    return true;
}
