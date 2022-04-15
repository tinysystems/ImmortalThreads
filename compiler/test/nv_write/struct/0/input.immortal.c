#include "immortality.h"

_immortal_function(test) {
  _begin(test);
  _def struct a {
    uint8_t a;
    uint32_t b;
  } data;

  while (1) {
    // expected: `_WR(data.a, 1);`
    _WR(data.a, 1);
    // expected: `_WR(data.b, 2);`
    _WR(data.b, 2);
    // expected: `_WR_SELF(uint8_t, data.a, data.a + 1);`
    _WR_SELF(uint8_t, data.a, data.a + 1);
    // expected: `_WR_SELF(uint32_t, data.b, data.b + data.a);`
    _WR_SELF(uint32_t, data.b, data.b + data.a);
  }
  _end(test);
}

