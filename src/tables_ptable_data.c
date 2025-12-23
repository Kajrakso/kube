#include "tables_ptable_data.h"

ptable_data_t ptable_data_opt1 = {
    .name = "optimal1",
    .ptable_size = 10066636800ULL / 2, /* two elements per byte */
    .number_of_elements = 10066636800ULL,
    .filename = "H.dat",
    .cube_to_index_func = &cube_to_H_index,
    .gen_ptable_func = &gen_ptable_opt1,
    .read_value_ptable_func = &ptable_read_val_2_values_per_byte,
    .set_value_ptable_func = &ptable_set_val_2_values_per_byte,
    .ptable_is_loaded = false,
    .ptable = NULL              // be careful! always check if the table is loaded before use!
};

ptable_data_t ptable_data_dr = {
    .name = "dr",
    .ptable_size = 2217093120ULL / 2, /* two elements per byte */
    .number_of_elements = 2217093120ULL,
    .filename = "dr.dat",
    .cube_to_index_func = &cube_to_DR_index,
    .gen_ptable_func = &gen_ptable_DR,
    .read_value_ptable_func = &ptable_read_val_2_values_per_byte,
    .set_value_ptable_func = &ptable_set_val_2_values_per_byte,
    .ptable_is_loaded = false,
    .ptable = NULL              // be careful! always check if the table is loaded before use!
};

ptable_data_t* enabled_ptables[] = {
    &ptable_data_opt1,
    &ptable_data_dr
};




