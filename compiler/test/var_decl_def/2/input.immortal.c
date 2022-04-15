/**
 * \test More complex declaration statement with initializers in an immortal
 * function
 */
#include "immortality.h"

_immortal_function(test) {
  _begin(test);
  _def int a, b, *c;
  _WR(a, 0);
  _WR(b, 1);
  _WR(c, &a);
  _end(test);
}

