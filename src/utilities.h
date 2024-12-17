#ifndef _UTILITIES_H_
#define _UTILITIES_H_

#include <stdbool.h>
#include <stdint.h>

inline void util_cycle_elems_arr(uint16_t* arr, int* indeces, int num_indeces);
bool arr_eq(uint16_t* arr_a, uint16_t* arr_b, int n);

#endif