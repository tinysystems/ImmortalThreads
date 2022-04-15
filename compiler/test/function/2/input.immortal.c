/**
 * Test case: nested function calls
 */
#include "immortality.h"

#include <stdio.h>

_immortal_function_with_retval(foo, uint32_t, uint32_t a, uint32_t b) {
  _begin(foo);
  _return(foo, a + b);
  _end(foo);
}
uint32_t bar(uint32_t a, uint32_t b) __attribute__((
    annotate("immortalc::fn_idempotent"))) /*immortalc_fn_idempotent*/ {
  return a + b;
}
_immortal_function_with_retval(baz, uint32_t, uint32_t a, uint32_t b) {
  _begin(baz);
  _return(baz, a + b);
  _end(baz);
}

_immortal_function(test_nested_function_calls) {
  _begin(test_nested_function_calls);
  _def uint32_t sum;
  _def uint32_t a;
  _WR(a, 0);
  _def uint32_t b;
  _WR(b, 1);
  _def uint32_t c;
  _WR(c, 1);

  while (1) {
    {
      _def uint32_t tmp_var_0;
      _call(baz, &tmp_var_0, a, c);
      _call(foo, 0, bar(a, b), tmp_var_0);
    }
    {
      _WR(c, bar(a, b));
      _def uint32_t tmp_var_0;
      _call(baz, &tmp_var_0, a, b);
      _call(foo, 0, c, tmp_var_0);
    }
    _call(baz, &c, a, b);
    _call(foo, 0, bar(a, b), c);
  }
  _end(test_nested_function_calls);
}

