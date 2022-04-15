/**
 * \test Test case: operation on return value
 */
#include "immortality.h"

#include <stdio.h>

int foo(int a) {
  uint32_t f = 0;
  return f;
}

void TEST(void) {
  int a = 0;
  a = foo(2) + 1;
  short b = 0;
  // implicit cast
  b = foo(2);
}
