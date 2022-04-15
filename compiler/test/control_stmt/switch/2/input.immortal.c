/**
 * \test Switch statement with mixed break and empty fallthrough
 */
#include "immortality.h"

#include <stdio.h>

_immortal_function(switch_transformation_test) {
  _begin(switch_transformation_test);
  _def size_t i;
  _WR(i, 0);
  if (i == 123) {
    _WR(i, 123);
  } else if (i == 0 || i == 1) {
    _WR(i, 1);
  } else if (i == 2 || i == 3) {
    _WR(i, 1);
  } else {
    _WR(i, 3);
  }
  _end(switch_transformation_test);
}

