#include "../src/utilities.h"
#include "criterion/criterion.h"
#include "criterion/redirect.h"
#include <stdint.h>

// suites

TestSuite(util_tests);
TestSuite(mod_arithmetic);


// tests

Test(util_tests, cycle_elems){
  uint16_t arr[] = {1, 2, 3, 4};
  int indeces[] = {2, 3};
  uint16_t expected_arr[] = {1, 2, 4, 3};

  util_cycle_elems_arr(arr, indeces, 2);
  cr_assert(arr_eq(arr, expected_arr, 4));
}

Test(util_tests, cycle_elems2){
  uint16_t arr2[] = {5, 6, 7, 8, 10, 14, 9, 0};
  int indeces2[] = {1, 4, 5, 6};
  uint16_t expected_arr2[] = {5, 9, 7, 8, 6, 10, 14, 0};

  util_cycle_elems_arr(arr2, indeces2, 4);
  cr_assert(arr_eq(arr2, expected_arr2, 8));
}