#include "prime.h"

#include <math.h>
#include <stdbool.h>


bool is_prime(const size_t x) {
  if (x <= 2) {
    return false;
  }
  if (x % 2 == 0) {
    return false;
  }
  for (size_t i = 3; i < (size_t)floor(sqrt(x)); i++) {
    if (x % i == 0) {
      return false;
    }
  }
  return true;
}

size_t prime_find_next(size_t x) {
  x++;
  while (!is_prime(x)) {
    x++;
  }
  return x;
}
