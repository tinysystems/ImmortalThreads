#include "immortality.h"

_immortal_function(for_loop_test) {
  _begin(for_loop_test);
  {
    _def size_t i;
    _WR(i, 0);
    for (; i < 6;) {
      _WR_SELF(size_t, i, i + 1);
    }
  }

  {
    _def size_t i, j;
    _WR(i, 0);
    _WR(j, 0);
    for (; i < 6;) {
      {
        _def size_t k, l;
        _WR(k, 0);
        _WR(l, 0);
        for (; k < 6;) {
          _WR_SELF(size_t, k, k + 1);
          _WR_SELF(size_t, l, l + 1);
          k, l;
        }
      }
      _WR_SELF(size_t, i, i + 1);
      _WR_SELF(size_t, j, j + 1);
      i, j;
    }
  }

  // typical infinite for loop that doesn't need to be decomposed
  _def size_t foo;
  _WR(foo, 0);
  for (;;) {
    _WR_SELF(size_t, foo, foo + 1);
  }
  _end(for_loop_test);
}

