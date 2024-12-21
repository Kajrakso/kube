#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdbool.h>
#include <stdint.h>

/* Quick helper function that cheks if
 two arrays of the same size is equal. */
bool arr_eq(uint16_t* arr_a, uint16_t* arr_b, int n);

#endif /* _UTILS_H_ */