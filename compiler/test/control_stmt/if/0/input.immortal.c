#include "immortality.h"

#include <stdio.h>

_immortal_function(if_decomposition_test) {
  _begin(if_decomposition_test);
  _def size_t i;
  _WR(i, 0);
  {
    _def char condition_tmp;
    _WR_SELF(size_t, i, i + 1);
    _WR(condition_tmp, (i == 1));
    if (condition_tmp) {
      _WR(i, 0);
    }
  }

  {
    _def char condition_tmp;
    {
      _def size_t tmp_var_0;
      _WR(tmp_var_0, i);
      _WR_SELF(size_t, i, i + 1);
      _WR(condition_tmp, (tmp_var_0 == 1));
    }
    if (condition_tmp) {
      _WR(i, 0);
    }
  }

  // should not be instrumented
  if (i == 1) {
    _WR(i, 0);
  }
  _end(if_decomposition_test);
}

