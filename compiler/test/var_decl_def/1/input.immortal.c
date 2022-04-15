/**
 * \test A simple variable declaration with initializer in an immortal function
 */
#include "immortality.h"

_immortal_function(test) {
  _begin(test);
  _def int a;
  _WR(a, 0);
  _end(test);
}

