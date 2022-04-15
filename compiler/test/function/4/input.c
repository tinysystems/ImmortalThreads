/**
 * \test Test case: complex return value statement, with side effects
 */
#include "immortality.h"

#include <stdio.h>

int foo(int a) {
  uint32_t f = 0;
  return f;
}

int TEST(void) {
  int a = 0;
  return foo(foo(foo(a)));
}
