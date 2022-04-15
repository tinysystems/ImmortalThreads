#include "immortality.h"

_immortal_function(test) {
  _begin(test);
  _def uint32_t a, b, c;
  _WR(c, 5);
  _WR(b, c);
  _WR(a, b);
  _end(test);
}

