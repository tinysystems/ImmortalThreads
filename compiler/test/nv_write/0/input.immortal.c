#include "immortality.h"

_immortal_function(test) {
  _begin(test);
  _def uint32_t sum;
  _def uint8_t cnt;

  while (1) {
    // expected: `_WR(cnt, 1);`
    _WR(cnt, 1);
    // expected: `_WR(cnt, 2);`
    _WR(cnt, 2);
    // expected: `_WR_SELF(uint8_t, cnt, cnt + 1);`
    _WR_SELF(uint8_t, cnt, cnt + 1);
    // expected: `_WR_SELF(uint32_t, sum, sum + cnt);`
    _WR_SELF(uint32_t, sum, sum + cnt);
  }
  _end(test);
}

