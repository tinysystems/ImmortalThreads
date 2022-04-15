/**
 * Test case: application developer marking idempotent functions
 *
 * Idempotent functions should not be instrumented
 */
#include "immortality.h"

#include <stdio.h>

int foo() __attribute__((
    annotate("immortalc::fn_idempotent"))) /*immortalc_fn_idempotent*/ {
  uint32_t f = 0;
  f = 5;
  f += 5;
  ++f;
  return f;
}

_immortal_function_with_retval(bar, int, int a) {
  _begin(bar);
  _def uint32_t f;
  _return(bar, f);
  _end(bar);
}

_immortal_function(TEST) {
  _begin(TEST);
  _def int a;
  _WR(a, foo());
  foo();

  _def int b;
  _call(bar, &b, foo());
  _call(bar, 0, foo());

  _WR(b, foo());
  _WR(a, b);
  _end(TEST);
}

