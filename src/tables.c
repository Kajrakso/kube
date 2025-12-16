#include <fcntl.h>  // file control
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "tables.h"
#include "tables_ptable_data.h"

int init_table(const char* path, size_t table_size, void** table_ptr);

// The table pointers are only visible in this file.
static void* ptable_H          = NULL;
static void* sym_table_e_index = NULL;

void* mmap_table(const char* path, size_t* out_size) {
    int fd = open(path, O_RDONLY);
    if (fd < 0)
        return NULL;

    struct stat st;
    if (fstat(fd, &st) < 0)
    {
        close(fd);
        return NULL;
    }

    *out_size    = st.st_size;
    void* mapped = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);

    if (mapped == MAP_FAILED)
        return NULL;
    return mapped;
}

void* load_table_from_file(const char* path, const size_t table_size) {
    FILE* fp = fopen(path, "rb");
    if (!fp)
        return NULL;
    void* buffer = malloc(table_size);
    if (!buffer)
    {
        fclose(fp);
        return NULL;
    }
    fread(buffer, 1, table_size, fp);
    fclose(fp);
    return buffer;
}

bool save_table_to_file(const char* path, void* table, const size_t table_size) {
    fprintf(stderr, "writing table to disk. file: %s.\n", path);

    FILE* fp = fopen(path, "wb");
    if (!fp)
        return false;

    if (fwrite(table, table_size, 1, fp) != 1)
    {
        fclose(fp);
        return false;
    }

    fclose(fp);
    return true;
}

int init_table(const char* path, size_t table_size, void** table_ptr) {
    size_t out_size = 0;
    if (*table_ptr == NULL)
    {
        void* table_data = mmap_table(path, &out_size);
        if (out_size != table_size)
        {
            munmap(table_data, table_size);
            return 1;
        }
        *table_ptr = table_data;
        return 0;
    }
    return 0;
}

void* get_ptable_H() { return ptable_H; }

void* get_sym_table_e_index() { return sym_table_e_index; }

void free_ptable(ptable_data_t* ptable_data) {
    munmap(ptable_data->ptable, ptable_data->ptable_size);
    ptable_data->ptable_is_loaded = false;
    ptable_data->ptable = NULL;
}

void cube_tables_generate() {
    // gen moves
    initialize_move_tables();
    gen_move_tables();

    // gen sym
    initialize_sym_tables();
    gen_sym_tables();

    gen_move_mask();

    // temp: combinatorials
    precompute_combinatorials();

    // temp: gen c_sym_cclass
    gen_c_sym_index_tables();
}


int cube_tables_load_ptable(ptable_data_t* ptable_data) {
    if (ptable_data->ptable_is_loaded){
        fprintf(stderr, "ptable %s is already loaded. skipping...\n", ptable_data->filename);
        return 0;
    }

    char fname[strlen(tabledir) + FILENAME_MAX];

    strcpy(fname, tabledir);
    strcat(fname, "/");
    strcat(fname, ptable_data->filename);

    if (init_table(fname, ptable_data->ptable_size, &ptable_data->ptable) != 0)
    {
        fprintf(stderr, "Failed to load pruning table H.\n");
        return 1;
    }
    ptable_data->ptable_is_loaded = true;
    return 0;
}

int cube_tables_load_sym_table_e_index() {
    char fname[strlen(tabledir) + FILENAME_MAX];

    strcpy(fname, tabledir);
    strcat(fname, "/");
    strcat(fname, "sym_table_e_index.dat");

    if (init_table(fname, sizeof(uint64_t) * NECE * NEO * NSYMS, &sym_table_e_index) != 0)
    {
        fprintf(stderr, "Failed to load sym_table_e_index.\n");
        return 1;
    }
    return 0;
}

int cube_tables_load() {
    if (cube_tables_load_ptable(&ptable_data_opt1) == 1)
        return 1;
    if (cube_tables_load_sym_table_e_index() == 1)
        return 1;
    return 0;
}

void cube_tables_free() {
    free_ptable(&ptable_data_opt1);

    munmap(sym_table_e_index, sizeof(uint64_t) * NECE * NEO * NSYMS);
    sym_table_e_index = NULL;
}
