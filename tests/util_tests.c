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
