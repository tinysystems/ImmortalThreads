/**
 * Test case: struct with array members
 */
#include "immortality.h"

void test_array_of_structs() {
  struct a {
    uint8_t a;
    uint32_t b;
  } data[10];

  while (1) {
    // expected: `_WR(data[0].a, 1);`
    data[0].a = 1;
    // expected: `_WR(data[1].b, 2);`
    data[1].b = 2;
    // expected: `_WR_SELF(uint8_t, data.a, data.a + 1);`
    data[0].a = data[0].a + 1;
    // expected: `_WR_SELF(uint32_t, data.b, data.b + data.a);`
    data[0].a = data[1].a + 1;
  }
}
