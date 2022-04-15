#include "immortality.h"

_immortal_function(test) {
  _begin(test);
  _def struct {
    uint8_t a;
    uint32_t b;
    struct {
      uint8_t a;
      uint32_t b;
    } c;
  } data;

  while (1) {
    // expected: `_WR(data.c.a, 1);`
    _WR(data.c.a, 1);
    // expected: `_WR(data.c.b, 2);`
    _WR(data.c.b, 2);
    // expected: `_WR_SELF(uint8_t, data.c.a, data.c.a + 1);`
    _WR_SELF(uint8_t, data.c.a, data.c.a + 1);
    // expected: `_WR_SELF(uint32_t, data.c.b, data.c.b + data.a);`
    _WR_SELF(uint32_t, data.c.b, data.c.b + data.c.a);
  }
  _end(test);
}

