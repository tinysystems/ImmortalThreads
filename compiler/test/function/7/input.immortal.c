/**
 * \test Test case: function call in ternary expressions and with ternary
 * expressions in parameter list
 */
#include "immortality.h"

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
  if (a >= 0) {
    _call(foo, 0, 1);
  } else {
    _call(foo, 0, 2);
  };

  {
    _def int _ternary_tmp_var;
    if (a >= 0) {
      _call(foo, &_ternary_tmp_var, 1);
    } else {
      _call(foo, &_ternary_tmp_var, 2);
    }
    _call(foo, 0, _ternary_tmp_var);
  }
  _end(TEST);
}

