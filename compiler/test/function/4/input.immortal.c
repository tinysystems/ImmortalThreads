/**
 * \test Test case: complex return value statement, with side effects
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

_immortal_function_with_retval(TEST, int) {
  _begin(TEST);
  _def int a;
  _WR(a, 0);
  {
    _def int tmp_var_0;
    _call(foo, &tmp_var_0, a);
    _def int tmp_var_1;
    _call(foo, &tmp_var_1, tmp_var_0);
    _def int tmp_var_2;
    _call(foo, &tmp_var_2, tmp_var_1);
    _return(TEST, tmp_var_2);
  }
  _end(TEST);
}

