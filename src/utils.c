#include "utils.h"

bool arr_eq(uint16_t* arr_a, uint16_t* arr_b, int n){
  for (int i = 0; i < n; i++){
    if (arr_a[i] != arr_b[i]) return false;
  }
  return true;
}
