/**
 * \test Test case: same parameter and return value
 */
#include "immortality.h"

#include <stdio.h>

int foo(int a) { return a + 1; }

void TEST(void) {
  int a = 0;
  a = foo(a);
  if (1) {
    // inside nested block
    a = foo(a);
  }
}
