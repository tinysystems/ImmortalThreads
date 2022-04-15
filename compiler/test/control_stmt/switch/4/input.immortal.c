/**
 * \test The case statements contain compound statements
 */
#include "immortality.h"

#include <stdio.h>

_immortal_function(switch_transformation_test) {
  _begin(switch_transformation_test);
  _def size_t i;
  _WR(i, 0);
  if (i == 0) {
    {
      _def int a;
      _WR(a, 0);
      _WR(i, 0);
    };
  } else if (i == 1) {
    _WR(i, 1);
  } else if (i == 2) {
    { _WR(i, 2); };
  }
  _end(switch_transformation_test);
}

