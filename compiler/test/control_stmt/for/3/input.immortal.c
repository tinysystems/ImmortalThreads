#include "immortality.h"

_immortal_function(for_loop_test) {
  _begin(for_loop_test);
  _def size_t i;
  _WR(i, 0);
  /*
   * shouldn't bring `i` in init stmt position out, since it is not a
   * declaration
   */
  for (i; i < 6;) {
    _WR_SELF(size_t, i, i + 1);
  }
  _end(for_loop_test);
}

