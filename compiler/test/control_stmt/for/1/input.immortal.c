#include "immortality.h"

_immortal_function(for_loop_test) {
  _begin(for_loop_test);
  {
    _def size_t foo;
    _WR(foo, 0);
    {
      _def char condition_tmp;
      _WR_SELF(size_t, foo, foo + 1);
      _WR(condition_tmp, (foo == 100));
      for (; condition_tmp;) {
        _WR_SELF(size_t, foo, foo - 1);
        _WR_SELF(size_t, foo, foo + 1);
        _WR_SELF(size_t, foo, foo - 1);
        _WR_SELF(size_t, foo, foo + 1);
        _WR(condition_tmp, foo == 100);
      }
    }
  }
  {
    _def size_t foo;
    _WR(foo, 0);
    for (;;) {
      _WR_SELF(size_t, foo, foo - 1);
      _WR_SELF(size_t, foo, foo + 1);
      _WR_SELF(size_t, foo, foo - 1);
      _WR_SELF(size_t, foo, foo + 1);
    }
  }
  _end(for_loop_test);
}

