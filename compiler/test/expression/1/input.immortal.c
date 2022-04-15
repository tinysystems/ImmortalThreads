#include "immortality.h"

_immortal_function(test) {
  _begin(test);
  _def uint32_t a, b;
  _WR(a, 0);
  _WR(b, 0);

  {
    _def uint32_t tmp_var_0;
    _WR(tmp_var_0, a);
    _WR_SELF(uint32_t, a, a + 1);
    _def uint32_t tmp_var_1;
    _WR(tmp_var_1, b);
    _WR_SELF(uint32_t, b, b + 1);
    tmp_var_0, tmp_var_1;
  }
  _end(test);
}

