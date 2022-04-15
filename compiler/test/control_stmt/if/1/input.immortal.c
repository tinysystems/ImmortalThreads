#include "immortality.h"

#include <stdio.h>

_immortal_function(if_wihout_brace_test) {
  _begin(if_wihout_brace_test);
  _def size_t i;
  _WR(i, 100);
  // Intentional misleading indentation
  // clang-format off
  if ( 1  /*true*/) {
    _WR(i, 0);
}
    _WR(i, 0);
// clang-format on
_end(if_wihout_brace_test);
}

