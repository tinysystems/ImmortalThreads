/**
 * Test case: simple function calls with
 */
#include "immortality.h"

#include <stdio.h>

void bar() {
}

int baz(int a) { return a; }

void TEST(void) {
  bar();

  int a;
  int c = baz(a);
  baz(a);
}
