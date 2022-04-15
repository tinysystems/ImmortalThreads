/**
 * \test Test case: operation on return value
 */
#include "immortality.h"

#include <stdio.h>

_immortal_function_with_retval(foo, int, int a) {
  _begin(foo);
  _def uint32_t f;
  _WR(f, 0);
  _return(foo, f);
  _end(foo);
}

_immortal_function(TEST) {
  _begin(TEST);
  _def int a;
  _WR(a, 0);
  {
    _def int tmp_var_0;
    _call(foo, &tmp_var_0, 2);
    _WR(a, tmp_var_0 + 1);
  }
  _def short b;
  _WR(b, 0);
  // implicit cast
  {
    _def int tmp_var_0;
    _call(foo, &tmp_var_0, 2);
    _WR(b, tmp_var_0);
  }
  _end(TEST);
}

