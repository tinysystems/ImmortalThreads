/**
 * Test case: don't instrument non-immortal function
 */
#include "immortality.h"

#include <stdio.h>

void bar() immortalc_fn_ignore {}

int b = 0;

void TEST(void) immortalc_fn_ignore {
  b = 0;
  int i = 0;
  ++i;
  bar();
  i > 0 ? ++i : i++;

  switch (++i) {
  case 0:
    break;
  case 1:
    break;
  }
}
