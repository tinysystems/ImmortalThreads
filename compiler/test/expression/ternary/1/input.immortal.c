/**
 * \test Test case: ternary expression within parenthesis
 *
 * When transformed into if/else statements, the parenthesis must be properly
 * handled.
 */
#include "immortality.h"

void foo(void)
    __attribute__((annotate("immortalc::ignore"))) /*immortalc_fn_ignore*/;

_immortal_function(test_ternary) {
  _begin(test_ternary);
  _def uint32_t a;
  _WR(a, 0);
  _def uint32_t b;
  _WR(b, 1);
  _def uint32_t c;
  _WR(c, 1);

  {
    _def unsigned int _ternary_tmp_var;
    {
      _def char condition_tmp;
      _WR_SELF(uint32_t, b, b + 1);
      _WR(condition_tmp, (b >= 1));
      if (condition_tmp) {
        _WR(_ternary_tmp_var, b);
      } else {
        _WR(_ternary_tmp_var, c);
      }
    }
    (_ternary_tmp_var);
  }

  // this ternary expression is special in that it evaluates to void (since foo
  // returns void)
  {
    _def char condition_tmp;
    _WR_SELF(uint32_t, b, b + 1);
    _WR(condition_tmp, (b >= 1));
    if (condition_tmp) {
      foo();
    } else {
      c;
    }
  };
  _end(test_ternary);
}

