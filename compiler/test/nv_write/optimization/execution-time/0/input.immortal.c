/**
 * \test _WR coalescence
 */
#include "immortality.h"

_immortal_function(test) {
  _begin(test);
  _def uint32_t a;
  _WR(a, 0);
  _def uint8_t b;
  b = 0;
  _def uint8_t c;
  c = 0;
  _def uint8_t d;
  d = 0;

  while (1) {
    _WR(a, 1);
    b = 2;
    c = 3;
    d = 4;
  }
  _end(test);
}

