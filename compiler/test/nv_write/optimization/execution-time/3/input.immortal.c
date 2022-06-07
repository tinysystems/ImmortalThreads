/**
 * \test _WR coalescence when pointers are involved.
 */
#include "immortality.h"

_immortal_function(test) {
  _begin(test);
  _def struct {
    uint8_t a;
    uint32_t b;
    struct {
      uint8_t a;
      uint32_t b;
      uint32_t c;
    } c;
  } data;

  _def uint32_t *ptr;
  _WR(ptr, &data.c.b);

  while (1) {
    _WR(data.c.a, 1);
    _WR_SELF(uint32_t, *ptr, *ptr + 1);
    _WR(data.c.c, 2);
  }
  _end(test);
}

