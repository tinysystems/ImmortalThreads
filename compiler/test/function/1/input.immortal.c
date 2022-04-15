/**
 * Test case: function call expression with side effects in the arguemnt list
 * expressions
 */
#include "immortality.h"

#include <stdio.h>

_immortal_function(foo, uint32_t a, uint32_t b) {
  _begin(foo);
  printf("%u%u", (unsigned)a, (unsigned)b);
  _return_void(foo);
  _end(foo);
}

_immortal_function(TEST) {
  _begin(TEST);
  _def uint32_t sum;
  _def uint32_t a, b, c;
  _WR(a, 0);
  _WR(b, 1);
  _WR(c, 1);

  while (1) {
    // call expression with side effects
    {
      _def uint32_t tmp_var_0;
      _WR(tmp_var_0, a);
      _WR_SELF(uint32_t, a, a + 1);
      _def uint32_t tmp_var_1;
      _WR(tmp_var_1, b);
      _WR_SELF(uint32_t, b, b + 1);
      _call(foo, tmp_var_0, tmp_var_1);
    }
  }
  _end(TEST);
}

