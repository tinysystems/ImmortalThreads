#include "immortality.h"

_immortal_function(test) {
  _begin(test);
  _def uint32_t a;
  _WR(a, 0);

  while (1) {
    _WR(a, 5);
    _WR(a, 6);
    a, a;
  }
  _end(test);
}

