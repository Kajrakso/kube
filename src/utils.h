#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/* Checks if two arrays are equal. */
bool arr_eq(uint16_t* arr_a, uint16_t* arr_b, int n);

/* Checks if the parity of a permutation is even. */
bool arr_perm_parity_is_even(uint16_t* arr, int n);

/* Shuffles an array. */
void arr_shuffle(uint16_t* arr, int n);

/* Swaps two uint16_t values. */
void swap_uint16(uint16_t* a, uint16_t* b);

#endif /* _UTILS_H_ */