#include "immortality.h"

void test() {
  uint32_t sum;
  uint8_t cnt;

  while (1) {
    // expected: `_WR(cnt, 1);`
    cnt = 1;
    // expected: `_WR(cnt, 2);`
    cnt = 2;
    // expected: `_WR_SELF(uint8_t, cnt, cnt + 1);`
    cnt = cnt + 1;
    // expected: `_WR_SELF(uint32_t, sum, sum + cnt);`
    sum = sum + cnt;
  }
}
