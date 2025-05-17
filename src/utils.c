#include "utils.h"

bool
arr_eq(uint16_t* arr_a, uint16_t* arr_b, int n){
  for (int i = 0; i < n; i++){
    if (arr_a[i] != arr_b[i]) return false;
  }
  return true;
}


bool
arr_eq_int(int* arr_a, int* arr_b, int n){
  for (int i = 0; i < n; i++){
    if (arr_a[i] != arr_b[i]) return false;
  }
  return true;
}

bool
arr_perm_parity_is_even(uint16_t* arr, int n){
  int parity = 0;
  for (int i = 0; i < n; i++){
    for (int j = i + 1; j < n; j++){
      if (arr[i] > arr[j]) parity++;
    }
  }
  return parity % 2 == 0;
}

void
arr_shuffle(uint16_t* arr, int n){
  for (int i = n - 1; i > 0; i--){
    int j = rand() % (i + 1);
    swap_uint16(&arr[i], &arr[j]);
  }
}

void
swap_uint16(uint16_t* a, uint16_t* b){
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

int
comb(int n, int k){
  if (k > n) return 0;
  else return factorial(n) / (factorial(n - k) * factorial(k));
}

int
perm_to_fact(int* p, int n){
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


int ece_combinatorials_lookup[20736];
int ccu_combinatorials_lookup[4096];

// naive way of precomputing the combinatorials, but
// it lets me look up the e slice edges directly without any sorting
void
precompute_combinatorials(){
  // set dummy vals for those that are not set.
  for (int i = 0; i < 20736; i++){
    ece_combinatorials_lookup[i] = -1;
  }

  for (int i = 0; i < 4096; i++){
    ccu_combinatorials_lookup[i] = -1;
  }

    // precompute the 24 permutations of 4 letters.
    int perms[24][4];
    for (int i = 0; i < 24; i++){
        fact_to_perm(i, 4, perms[i]);
    }

    for (int c1 = 0; c1 < 9; c1++){
        for (int c2 = c1 + 1; c2 < 10; c2++){
            for (int c3 = c2 + 1; c3 < 11; c3++){
                for (int c4 = c3 + 1; c4 < 12; c4++){
                    // we have the combination
                    int arr[] = {c1, c2, c3, c4};

                    // and the combinatorial number
                    // https://en.wikipedia.org/wiki/Combinatorial_number_system
                    uint32_t c = comb(c1, 1) + comb(c2, 2) + comb(c3, 3) + comb(c4, 4);

                    // for all permutations of this combination, save the same value
                    for (int perm = 0; perm < 24; perm++){
                        ece_combinatorials_lookup[
                              arr[perms[perm][0]] * 1
                            + arr[perms[perm][1]] * 12
                            + arr[perms[perm][2]] * 12*12
                            + arr[perms[perm][3]] * 12*12*12
                        ] = c;
                    }
                }
            }
        }
    }    


    for (int c1 = 0; c1 < 5; c1++){
        for (int c2 = c1 + 1; c2 < 6; c2++){
            for (int c3 = c2 + 1; c3 < 7; c3++){
                for (int c4 = c3 + 1; c4 < 8; c4++){
                    // we have the combination
                    int arr[] = {c1, c2, c3, c4};

                    // and the combinatorial number
                    // https://en.wikipedia.org/wiki/Combinatorial_number_system
                    uint32_t c = comb(c1, 1) + comb(c2, 2) + comb(c3, 3) + comb(c4, 4);

                    // for all permutations of this combination, save the same value
                    for (int perm = 0; perm < 24; perm++){
                        ccu_combinatorials_lookup[
                              arr[perms[perm][0]] * 1
                            + arr[perms[perm][1]] * 8
                            + arr[perms[perm][2]] * 8*8
                            + arr[perms[perm][3]] * 8*8*8
                        ] = c;
                    }
                }
            }
        }
    }    
}
