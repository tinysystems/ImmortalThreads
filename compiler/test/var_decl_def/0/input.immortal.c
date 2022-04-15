/**
 * \test A simple variable declaration in an immortal function
 */
#include "immortality.h"

_immortal_function(test) {
  _begin(test);
  // expected: _def int a;
  _def int a;
  _end(test);
}

