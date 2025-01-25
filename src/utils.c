#include "utils.h"

bool arr_eq(uint16_t* arr_a, uint16_t* arr_b, int n){
  for (int i = 0; i < n; i++){
    if (arr_a[i] != arr_b[i]) return false;
  }
  return true;
}

bool arr_perm_parity_is_even(uint16_t* arr, int n){
  int parity = 0;
  for (int i = 0; i < n; i++){
    for (int j = i + 1; j < n; j++){
      if (arr[i] > arr[j]) parity++;
    }
  }
  return parity % 2 == 0;
}

void arr_shuffle(uint16_t* arr, int n){
  for (int i = n - 1; i > 0; i--){
    int j = rand() % (i + 1);
    swap_uint16(&arr[i], &arr[j]);
  }
}

void swap_uint16(uint16_t* a, uint16_t* b){
  uint16_t temp;

  temp = *a;
  *a = *b;
  *b = temp;
}

int
factorial(int n)
{
	int i, ret = 1;

	if (n < 0)
		return 0;

	for (i = 1; i <= n; i++)
		ret *= i;

	return ret;
}

int perm_to_fact(int* p, int n){
    int i, j, m, t = 0;

    for (i = 0; i < n; i++){
        m = 0;
        for (j = i + 1; j < n; j++){
            if (p[i] > p[j]) m++;
        }
        t += m * factorial(n - 1 - i);
    }

    return t;
}

void
fact_to_perm(int p, int n, int *r)
{
	int i, j, c;
	int a[n];

	for (i = 0; i < n; i++)
		a[i] = 0;

	if (p < 0 || p >= factorial(n))
		for (i = 0; i < n; i++)
			r[i] = -1;

	for (i = 0; i < n; i++) {
		c = 0;
		j = 0;
		while (c <= p / factorial(n-i-1))
			c += a[j++] ? 0 : 1;
		r[i] = j-1;
		a[j-1] = 1;
		p %= factorial(n-i-1);
	}
}