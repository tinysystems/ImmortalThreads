/**
 * Test case: function call expression with side effects in the arguemnt list
 * expressions
 */
#include "immortality.h"

#include <stdio.h>

void foo(uint32_t a, uint32_t b) {
  printf("%u%u", (unsigned)a, (unsigned)b);
  return;
}

void TEST(void) {
  uint32_t sum;
  uint32_t a = 0, b = 1, c = 1;

  while (1) {
    // call expression with side effects
    foo(a++, b++);
  }
}
