/**
 * Test case: shim API replacement of CHECKPOINT()
 */
#include "immortality.h"

#include <stdio.h>

_immortal_function(test) {
  _begin(test);
  _def int a;

  _WR_SELF(int, a, a + 0);
  _CHECKPOINT();
  _WR_SELF(int, a, a + 0);
  _end(test);
}

