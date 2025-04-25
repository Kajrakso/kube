#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/* Checks if two arrays are equal. */
bool arr_eq(uint16_t* arr_a, uint16_t* arr_b, int n);
bool arr_eq_int(int* arr_a, int* arr_b, int n);

/* Checks if the parity of a permutation is even. */
bool arr_perm_parity_is_even(uint16_t* arr, int n);

/* Shuffles an array. */
void arr_shuffle(uint16_t* arr, int n);

/* Swaps two uint16_t values. */
void swap_uint16(uint16_t* a, uint16_t* b);

/* Converts a permutation `p` of length `n` to a factoriadic. */
int perm_to_fact(int* p, int n);

/* Converts a factoriadic `p` to a permutation `r` of size `n`. */
void fact_to_perm(int p, int n, int *r);

/* this should be precomputed if used */
int comb(int n, int k);

/* */
void precompute_combinatorials();

#endif /* _UTILS_H_ */
