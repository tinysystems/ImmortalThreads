#include "immortality.h"

void test() {
  uint32_t a = 0;
  uint32_t b = 1;
  uint32_t c = 1;
  uint32_t *p = &a;

  while (1) {
    a = ++(*++(p)), ++(*++(p));
    a = c++, a = c++, ++b, ++c;
    a = b++ + ++c;
  }
}
