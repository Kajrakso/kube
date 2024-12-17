#include "utilities.h"

// note: we assume that all indeces are valid.
void util_cycle_elems_arr(uint16_t* arr, int* indeces, int num_indeces){
  int temp = arr[indeces[num_indeces - 1]];
  for (int i = num_indeces - 1; i > 0; i--){
    arr[indeces[i]] = arr[indeces[i - 1]];
  }
  arr[indeces[0]] = temp;
}

// quick helper function that cheks if
// two arrays of the same size is equal.
bool arr_eq(uint16_t* arr_a, uint16_t* arr_b, int n){
  for (int i = 0; i < n; i++){
    if (arr_a[i] != arr_b[i]) return false;
  }
  return true;
}
