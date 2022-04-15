#include "immortality.h"

void test() {
  uint32_t a = 0;

  while (1) {
    a = 5, a = 6;
  }
}
