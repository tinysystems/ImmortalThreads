/**
 * Test case: application developer marking idempotent functions
 *
 * Idempotent functions should not be instrumented
 */
#include "immortality.h"

#include <stdio.h>

int foo() immortalc_fn_idempotent {
  uint32_t f = 0;
  f = 5;
  f += 5;
  ++f;
  return f;
}

int bar(int a) {
  uint32_t f;
  return f;
}

void TEST(void) {
  int a = foo();
  foo();

  int b = bar(foo());
  bar(foo());

  a = b = foo();
}
