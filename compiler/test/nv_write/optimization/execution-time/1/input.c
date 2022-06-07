/**
 * \test Here we cannot coalesce _WR due to repeated WAR dependency between `a`
 * and `b`
 */
#include "immortality.h"

void test() {
  uint32_t a = 0;
  uint8_t b = 0;

  while (1) {
    a = b + 1;
    b = a + 1;
    a = b + 1;
    b = a + 1;
  }
}
