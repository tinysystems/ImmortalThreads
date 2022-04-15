#include "immortality.h"

#include <stdio.h>

_immortal_function(for_loop_without_braces_test) {
  _begin(for_loop_without_braces_test);
  {
    _def size_t i;
    _WR(i, 0);
    for (; i < 6;) {
      _WR_SELF(size_t, i, i - 1);
      _WR_SELF(size_t, i, i + 1);
    }
  }

  {
    _def size_t i;
    _WR(i, 0);
    {
      _def char condition_tmp;
      _WR_SELF(size_t, i, i + 1);
      _WR(condition_tmp, (i < 6));
      for (; condition_tmp;) {
        _WR_SELF(size_t, i, i - 1);
        _WR_SELF(size_t, i, i + 1);
        _WR(condition_tmp, i < 6);
      }
    }
  }
  _end(for_loop_without_braces_test);
}

