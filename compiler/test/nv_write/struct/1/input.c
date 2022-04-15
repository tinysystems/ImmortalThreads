#include "immortality.h"

void test() {
  struct {
    uint8_t a;
    uint32_t b;
    struct {
      uint8_t a;
      uint32_t b;
    } c;
  } data;

  while (1) {
    // expected: `_WR(data.c.a, 1);`
    data.c.a = 1;
    // expected: `_WR(data.c.b, 2);`
    data.c.b = 2;
    // expected: `_WR_SELF(uint8_t, data.c.a, data.c.a + 1);`
    data.c.a = data.c.a + 1;
    // expected: `_WR_SELF(uint32_t, data.c.b, data.c.b + data.a);`
    data.c.b = data.c.b + data.c.a;
  }
}

