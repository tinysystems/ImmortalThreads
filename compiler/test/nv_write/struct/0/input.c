#include "immortality.h"

void test() {
  struct a {
    uint8_t a;
    uint32_t b;
  } data;

  while (1) {
    // expected: `_WR(data.a, 1);`
    data.a = 1;
    // expected: `_WR(data.b, 2);`
    data.b = 2;
    // expected: `_WR_SELF(uint8_t, data.a, data.a + 1);`
    data.a = data.a + 1;
    // expected: `_WR_SELF(uint32_t, data.b, data.b + data.a);`
    data.b = data.b + data.a;
  }
}
