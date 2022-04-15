/**
 * \test Writes to immortal variables manually specified by the programmer
 * should also be instrumented.
 */
#include "immortality.h"

#define decl_a_b(_prefix_)                                                     \
  __gdef int _prefix_##_a = 1;                                                 \
  __gdef int _prefix_##_b = 1;

decl_a_b(hello);

__gdef int b = 1;
__ro_hifram const int c = 1;

void test() {
  hello_a = 1;
  hello_b = 1;
  b = 2;
}
