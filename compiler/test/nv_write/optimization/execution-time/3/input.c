/**
 * \test _WR coalescence when pointers are involved.
 */
#include "immortality.h"

void test() {
  struct {
    uint8_t a;
    uint32_t b;
    struct {
      uint8_t a;
      uint32_t b;
      uint32_t c;
    } c;
  } data;

  uint32_t *ptr = &data.c.b;

  while (1) {
    data.c.a = 1;
    *ptr = *ptr + 1;
    data.c.c = 2;
  }
}
