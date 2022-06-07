/**
 * \test Here we cannot coalesce _WR due to repeated WAR dependency between `a`
 * and `b`
 */
#include "immortality.h"

_immortal_function(test) {
  _begin(test);
  _def uint32_t a;
  _WR(a, 0);
  _def uint8_t b;
  b = 0;

  while (1) {
    _WR(a, b + 1);
    _WR(b, a + 1);
    _WR(a, b + 1);
    _WR(b, a + 1);
  }
  _end(test);
}

