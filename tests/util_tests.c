#include "../src/utils.h"
#include "criterion/criterion.h"
#include "criterion/redirect.h"
#include <stdint.h>

// suites

TestSuite(util_tests);

// tests

Test(util_tests, arr_eq){
  uint16_t arr1[] = {1, 2, 3, 4};
  uint16_t arr2[] = {1, 2, 3, 4};
  
  cr_assert(arr_eq(arr1, arr2, 4));

  uint16_t arr3[] = {1};
  uint16_t arr4[] = {1};

  cr_assert(arr_eq(arr3, arr4, 1));

  uint16_t arr5[] = {1, 2, 3, 4};
  uint16_t arr6[] = {1, 2, 4, 3};

  cr_assert_not(arr_eq(arr5, arr6, 4));

  uint16_t arr7[] = {1, 1, 1};
  uint16_t arr8[] = {2, 2, 2};

  cr_assert_not(arr_eq(arr7, arr8, 3));
}

Test(util_tests, perm_parity_is_even){
  uint16_t arr1[] = {1, 2, 3, 4};
  cr_assert(perm_parity_is_even(arr1, 4));

  uint16_t arr2[] = {1, 2, 4, 3};
  cr_assert_not(perm_parity_is_even(arr2, 4));
}

Test(util_tests, swap_uint16){
  uint16_t a = 1;
  uint16_t b = 2;

  swap_uint16(&a, &b);

  cr_assert_eq(a, 2);
  cr_assert_eq(b, 1);
}