/**
 * \test Simple switch statement made entirely of empty fallthrough
 */
#include "immortality.h"

#include <stdio.h>

_immortal_function(switch_transformation_test) {
  _begin(switch_transformation_test);
  _def size_t i;
  _WR(i, 0);
  { _WR(i, 3); }
  _end(switch_transformation_test);
}

