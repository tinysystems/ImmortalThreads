/**
 * \test Test case: ternary expression decomposition
 */
#include "immortality.h"

void test_ternary() {
  uint32_t a = 0;
  uint32_t b = 1;
  uint32_t c = 1;

  a = b >= 1 ? b : c;

  b >= 1 ? b++ : c;

  a = b >= 1 ? b++ : c;

  a = b >= 1 ? b : c++;

  a = b++ >= 1 ? ++b : ++c;

  if (1) {
    a = b++ >= 1 ? (++b ? ++b : ++c) : (++c ? ++c : ++b);
  }

  while (1) {
    b >= 1 ? (b >= 2 ? b : c) : (c >= 1 ? ++c : ++b);
  }
}
