/**
 * \test Test case: function call in ternary expressions and with ternary
 * expressions in parameter list
 */
#include "immortality.h"

int foo(int a) {
  uint32_t f = 0;
  return f;
}

void TEST(void) {
  int a = 0;
  a >= 0 ? foo(1) : foo(2);

  foo(a >= 0 ? foo(1) : foo(2));
}
