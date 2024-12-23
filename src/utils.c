#include "utils.h"

bool arr_eq(uint16_t* arr_a, uint16_t* arr_b, int n){
  for (int i = 0; i < n; i++){
    if (arr_a[i] != arr_b[i]) return false;
  }
  return true;
}

bool perm_parity_is_even(uint16_t* arr, int n){
  int parity = 0;
  for (int i = 0; i < n; i++){
    for (int j = i + 1; j < n; j++){
      if (arr[i] > arr[j]) parity++;
    }
  }
  return parity % 2 == 0;
}

void swap_uint16(uint16_t* a, uint16_t* b){
  int temp;

  temp = *a;
  *a = *b;
  *b = temp;
}
