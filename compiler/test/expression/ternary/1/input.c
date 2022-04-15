/**
 * \test Test case: ternary expression within parenthesis
 *
 * When transformed into if/else statements, the parenthesis must be properly
 * handled.
 */
#include "immortality.h"

void foo(void) immortalc_fn_ignore;

void test_ternary() {
  uint32_t a = 0;
  uint32_t b = 1;
  uint32_t c = 1;

  (++b >= 1 ? b : c);

  // this ternary expression is special in that it evaluates to void (since foo
  // returns void)
  (++b >= 1 ? foo() : c);
}
