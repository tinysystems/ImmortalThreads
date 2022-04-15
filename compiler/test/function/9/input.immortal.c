/**
 * Test case: don't instrument non-immortal function
 */
#include "immortality.h"

#include <stdio.h>

void bar()
    __attribute__((annotate("immortalc::ignore"))) /*immortalc_fn_ignore*/ {}

_gdef int b = 0;

void TEST(void)
    __attribute__((annotate("immortalc::ignore"))) /*immortalc_fn_ignore*/ {
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

